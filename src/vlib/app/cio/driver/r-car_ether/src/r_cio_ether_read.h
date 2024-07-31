/*************************************************************************************************************
* cio_ether_read_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/**@defgroup etherread Ether Driver data read
 * @ingroup cio_eth_driver
 *
 * @brief Read from the Ether drivers's receive buffer
 *
 * @{
 */



#ifndef R_CIO_ETHER_READ_H
#define  R_CIO_ETHER_READ_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
   Section: Global private functions
 */


/*******************************************************************************
 *  Function Name: R_CIO_ETHER_PRV_Read
 */
/**
 * @brief  Read from the message buffer assigned to the given instance
 *
 * The data will stored in the buffer given by Par1.
 * The lenghth of the data to read must be a multiple of the the defined
 * ether message length <r_cio_ether_Msg_t>
 *
 * @param[in] Addr       - Channel id of the given ETHER instance
 * @param[in] Par1       - Buffer address (destination)
 * @param[in] Par2       - length of data to read
 * @param[in] Par3       - optional parameter (unused)
 *
 * @return == 0 - OK
 * @return != 0 - NG
 */
ssize_t R_CIO_ETHER_PRV_Read(size_t Addr, void *Par1, void *Par2, void *Par3);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
