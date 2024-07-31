BASEDIR=$(pwd)
git clone https://github.com/FreeRTOS/FreeRTOS.git ${BASEDIR}/src/vlib/os/freertos/
cd ${BASEDIR}/src/vlib/os/freertos
git checkout 89e7bbe292afd3912d1f0b2402cc506878bad869
git apply ${BASEDIR}/patches/freertos.patch

cd ${BASEDIR}
git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git ${BASEDIR}/src/vlib/os/freertos/FreeRTOS/Source
cd ${BASEDIR}/src/vlib/os/freertos/FreeRTOS/Source
git checkout 10bbbcf0b9bcab1869b3bac3987d32116f351718
git apply ${BASEDIR}/patches/freertos-kernel.patch
cd ${BASEDIR}
git clone https://github.com/TrampolineRTOS/trampoline ${BASEDIR}/src/vlib/os/trampoline/
cd ${BASEDIR}/src/vlib/os/trampoline
git checkout 105957cec66723672573ba1fc45005d506af92c6
git apply ${BASEDIR}/patches/trampoline.patch

