/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#ifndef _APP_CLUSTER_H_
#define _APP_CLUSTER_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: Cluster Application Interface
*/

void RenderAppCluster(davehd_dev_t *Dev);

void InitAppCluster(davehd_dev_t *Dev);

void DeInitAppCluster(davehd_dev_t *Dev);


#ifdef __cplusplus
}
#endif

#endif /* _APP_CLUSTER_H_ */
