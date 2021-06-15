---@class udplib
local udp = {}

---@class UdpHandle:userdata UDP handle.
local handle = {}

---Open a UDP handle.
---@param domain string
---@return UdpHandle
function udp.open(domain) end

---Bind a UDP handle to a local IP address and port.
---@param addr string Local address to use.
---@param port integer Local UDP port number, in host order.
function handle:bind(addr, port) end

---Set the remote address and/or port.
---@param addr string Remote address to use.
---@param port integer Remote UDP port number, in host order.
function handle:connect(addr, port) end

---Send a packet.
---@param data string The data to be sent.
function handle:send(data) end

---Send a packet to remote addr and port.
---@param data string The data to be sent.
---@param addr string Remote address to use.
---@param port integer Remote UDP port number, in host order.
function handle:sendto(data, addr, port) end

---Set the receive callback.
---@param cb fun(data: string, from_addr: string, from_port: integer, arg?: any)
---@param arg any
function handle:setRecvCb(cb, arg) end

---Set the error callback.
---@param cb fun(err: integer, arg?: any)
---@param arg any
function handle:setErrCb(cb, arg) end

---Close the UDP handle.
function handle:close() end

return udp