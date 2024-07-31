#!/bin/bash
# Renesas R-Car CR7 Solution

# Default package dir is the current dir
if [[ -z "${PKG_RELEASE}" ]]; then
	PKG_RELEASE=../.
fi
PKG_RELEASE=$(readlink -f $PKG_RELEASE)

# Submodules init and update
git submodule update --init --recursive

# Checkout repos for Android 10
if [[ -n "${MYDROID_10}" ]]; then
	# Default working dir is $PKG_RELEASE/work
	if [[ -z "${WORK}" ]]; then
		WORK=./work
	fi
	WORK=$(readlink -f $WORK)
	mkdir -p ${WORK}/android-10

	MYDROID_10=$(readlink -f $MYDROID_10)
	ln -s ${MYDROID_10}/device/renesas/kernel ${WORK}/android-10/kernel
	ln -s ${MYDROID_10}/device/renesas/bootloaders/ipl ${WORK}/android-10/ipl
	ln -s ${MYDROID_10}/hardware/renesas/hal/hwcomposer ${WORK}/android-10/hwcomposer

	cd ${WORK}/android-10/kernel
	git am ${PKG_RELEASE}/src/android-10/kernel/*.patch

	cd ${WORK}/android-10/ipl
	git am ${PKG_RELEASE}/src/android-10/ipl/*.patch

	cd ${WORK}/android-10/hwcomposer
	git am ${PKG_RELEASE}/src/android-10/hwcomposer/*.patch

	cd ${WORK}
fi


# Below set of auxiliary functions and variables that are used to apply
# changes necessary for CR7 support to the default Android BSP.
CR7_SDK_AUTO_COMMIT_MESSAGE="This commit was made automatically by CR7 SDK script."
CR7_SDK_AUTO_BRANCH_NAME="tmp-cr7-sdk"

# Check if directory exists.
# The function expects two argument:
#  - path to the directory that tested;
#  - flag reflects if the directory has to be created.
cr7_sdk_check_directory_exists () {
	if [ $2 == "new" ]; then
		if ! mkdir -p $1; then
			echo "Unable to create $1!"
			return 1
		fi
	fi

	if [ ! -d $1 ]; then
		echo "Directory $1 does not exist!"
		return 2
	fi

	return 0
}

# Function checks if the directory is a git repository.
# If not, the function initialize a new git repository.
# The function expects one argument:
#  - path to the directory that tested.
cr7_sdk_ensure_directory_is_git () {
	pushd $1
	if [ "$(git rev-parse --is-inside-work-tree 2>/dev/null)" != "true" ]; then
		echo "Directory $1 is not a git repository."
		rm -rf .git
		if ! git init; then
			echo "Can not initialize git repository in $1!"
			return 22
		fi
		if ! git commit --allow-empty \
				-m "initial commit"$'\n\n'"${CR7_SDK_AUTO_COMMIT_MESSAGE}"; then
			echo "Can not make initial commit in $1!"
			return 1
		fi
	fi
	popd

	return 0
}

# Save all working changes in the git repository (including untracked files).
# This function is necessary to prepare repository for applying patches.
# The function expects one argument:
#  - path to the git repository.
cr7_sdk_save_working_changes () {
	pushd $1
	if [ ! -z "$(git status --porcelain)" ]; then
		if ! (git add -A && git commit \
				-m "save working changes"$'\n\n'"${CR7_SDK_AUTO_COMMIT_MESSAGE}"); then
			echo "Can not save working changes in $1!"
			return 1
		fi
	fi
	popd

	return 0
}

# Creates separate branch for CR7 SDK changes and checkout on it.
# The function expects one argument:
#  - path to the git project.
# If the branch is already exists the function returns an error.
cr7_sdk_checkout_topic_branch () {
	pushd $1
	if ! git checkout -b $CR7_SDK_AUTO_BRANCH_NAME; then
		echo "Unable to create branch in $1!"
		return 1
	fi
	popd

	return 0
}

# This is a core function that tries to apply a single git patch.
# The function expects three arguments:
#  - path to the git project being patched;
#  - path to the directory that contains the patch;
#  - name of the patch being applied.
# First the function tries to apply the patch via `git am`. If an error
# occurred the script tries to continue with a raw `patch` command.
# This approach helps automatically resolve issues when git can't find
# index for the patch, but patch itself (set of changes) can be applied.
cr7_sdk_apply_one_git_patch () {
	pushd $1
	if ! git am $2/$3; then
		echo "Some issues with applying $3."
		if patch -d $1 -p1 --merge --no-backup-if-mismatch -i $2/$3; then
			echo "Unable to apply patch $3 in a raw mode!"
			return 1
		fi
		if [ $(git diff --name-only --diff-filter=u | wc -l) -gt 0 ] ; then
			echo "Merge conflict for patch $3!"
			return 1
		fi
		if ! (git add -A && git am --continue); then
			echo "Can not applying patch $3 with git!"
			return 1
		fi
	fi
	popd

	return 0
}

# Apply all patches one by one for specified project.
# The function expects two arguments:
#  - path to the git project being patched;
#  - path to the directory that contains patches.
# Before applying the patches are sorted to with respect to the git
# naming rule. If some patch can't be successfully applied, the function
# interrupts further processing and returns an error.
cr7_sdk_apply_git_patches () {
	result=0
	for patch in `ls -1 $2 | sort -V`
	do
		cr7_sdk_apply_one_git_patch $1 $2 $patch || result=1
		if [ "$result" -ne 0 ]; then
			 break
		fi
	done

	return $result
}

# Auxiliary function that is used to print error message and immediately
# exit from the script. It should be called in case of unrecoverable
# error when further script execution is not possible or harmful.
# This function doesn't expect any arguments.
cr7_sdk_warn_and_exit () {
	echo "Unrecoverable error during the project's preparation! Abort!"
	exit 1
}

# Main function to apply changes necessary for CR7 SDK integration for
# one Android subproject.
# This function expects four arguments:
#  - path to the Android top directory;
#  - path to the patches top directory;
#  - project's subpath being updated;
#  - an optional flag reflects if the project already exists or should
#    be created, by default evaluated to `old`, that means that the
#    project is already exists.
# This function does some "unexpected" work under hood:
#  - for projects that are not under git control it initializes new
#    git repository;
#  - it saves all uncommitted changes and make an automatic commit;
#  - it creates new temporary branch for CR7 SDK and checkout to it.
cr7_sdk_prepare_project () {
	is_new=${4:-old}

	cr7_sdk_check_directory_exists  $1/$3 $is_new || cr7_sdk_warn_and_exit
	cr7_sdk_ensure_directory_is_git $1/$3 || cr7_sdk_warn_and_exit
	cr7_sdk_save_working_changes    $1/$3 || cr7_sdk_warn_and_exit
	cr7_sdk_checkout_topic_branch   $1/$3 || cr7_sdk_warn_and_exit
	cr7_sdk_apply_git_patches $1/$3 $2/$3 || cr7_sdk_warn_and_exit

	return 0
}


# Checkout repos for Android 11
if [[ -n "${MYDROID_11}" ]]; then
	cr7_sdk_prepare_project ${MYDROID_11} ${PKG_RELEASE}/src/android-11 device/renesas/kernel
	cr7_sdk_prepare_project ${MYDROID_11} ${PKG_RELEASE}/src/android-11 device/renesas/bootloaders/u-boot
	cr7_sdk_prepare_project ${MYDROID_11} ${PKG_RELEASE}/src/android-11 device/renesas/bootloaders/ipl
	cr7_sdk_prepare_project ${MYDROID_11} ${PKG_RELEASE}/src/android-11 hardware/renesas/hal/hwcomposer
	cr7_sdk_prepare_project ${MYDROID_11} ${PKG_RELEASE}/src/android-11 vendor/renesas/apps/evs
	cr7_sdk_prepare_project ${MYDROID_11} ${PKG_RELEASE}/src/android-11 vendor/renesas/hal/evs
	cr7_sdk_prepare_project ${MYDROID_11} ${PKG_RELEASE}/src/android-11 device/renesas/common
	cr7_sdk_prepare_project ${MYDROID_11} ${PKG_RELEASE}/src/android-11 device/renesas/salvator
	cr7_sdk_prepare_project ${MYDROID_11} ${PKG_RELEASE}/src/android-11 vendor/renesas/firmware/remoteproc "new"
	cr7_sdk_prepare_project ${MYDROID_11} ${PKG_RELEASE}/src/android-11 hardware/renesas/modules/uvcs
fi


# Checkout repos for Android 13
if [[ -n "${MYDROID_13}" ]]; then
	cr7_sdk_prepare_project ${MYDROID_13} ${PKG_RELEASE}/src/android-13 device/renesas/kernel
	cr7_sdk_prepare_project ${MYDROID_13} ${PKG_RELEASE}/src/android-13 device/renesas/bootloaders/u-boot
	cr7_sdk_prepare_project ${MYDROID_13} ${PKG_RELEASE}/src/android-13 device/renesas/bootloaders/ipl
	cr7_sdk_prepare_project ${MYDROID_13} ${PKG_RELEASE}/src/android-13 hardware/renesas/hal/hwcomposer
	cr7_sdk_prepare_project ${MYDROID_13} ${PKG_RELEASE}/src/android-13 vendor/renesas/apps/evs
	cr7_sdk_prepare_project ${MYDROID_13} ${PKG_RELEASE}/src/android-13 device/renesas/common
	cr7_sdk_prepare_project ${MYDROID_13} ${PKG_RELEASE}/src/android-13 vendor/renesas/firmware/remoteproc "new"
	cr7_sdk_prepare_project ${MYDROID_13} ${PKG_RELEASE}/src/android-13 vendor/renesas/hal/evs
	cr7_sdk_prepare_project ${MYDROID_13} ${PKG_RELEASE}/src/android-13 vendor/renesas/hal/vehicle
fi
