#ifndef MACRO_GPU_DAVEHD_INTEGRITY_SERVER_SRC_R_DAVEHD_SERVER_DESERIALIZE_H_
#define MACRO_GPU_DAVEHD_INTEGRITY_SERVER_SRC_R_DAVEHD_SERVER_DESERIALIZE_H_


/*******************************************************************************
  Function: dhd_receive_call
  
  Executes the process corresponding to the received message. 
  The processing result is returned as a response message.
  
  Parameters:
  Client - Pointer of DHD Client instance

  Returns:
  E_DHD_OK on success E_DHD_ERROR otherwise

*/
int32_t dhd_receive_call(DHDClient_t *Client);

#endif /* MACRO_GPU_DAVEHD_INTEGRITY_SERVER_SRC_R_DAVEHD_SERVER_DESERIALIZE_H_ */
