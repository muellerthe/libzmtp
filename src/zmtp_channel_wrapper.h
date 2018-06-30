#include "net/gnrc/tcp.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Closes a TCP connection.
 *
 * @param[in,out] tcb       TCB holding the connection information.
 * 
 * @returns   Zero on succeess.
 */
int close_connection(gnrc_tcp_tcb_t *tcb);


/**
 * @brief Transmits data to connected peer.
 *
 * @param[in,out] tcb       TCB holding the connection information.
 * @param[in]     buf       Pointer to the buffer that should be transmitted.
 * @param[in]     len       Number of bytes that should be transmitted.
 *
 * @returns   The number of successfully transmitted bytes.
 *            -1 if an error occured.
 */
ssize_t send(gnrc_tcp_tcb_t *tcb, const void *buf, size_t len);


/**
 * @brief Receives data from the peer.
 *
 * @param[in,out] tcb       TCB holding the connection information.
 * @param[out]    buf       Pointer to the buffer where the received data should be copied into.
 * @param[in]     len       Length of the buffer pointed to by the buf argument.
 *
 * @returns   The number of bytes read into @p buf.
 *            -1 if an error occured.
 */
ssize_t recv(gnrc_tcp_tcb_t *tcb, void *buf, size_t len);


#ifdef __cplusplus
}
#endif