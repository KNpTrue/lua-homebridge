return {
    ---New a ``RelativeHumidityHumidifierThreshold`` characteristic.
    ---@param iid integer Instance ID.
    ---@param read fun(request:HapCharacteristicReadRequest): any
    ---@param write fun(request:HapCharacteristicWriteRequest, value:any)
    ---@return HapCharacteristic characteristic
    new = function (iid, read, write)
        return {
            format = "Float",
            iid = iid,
            type = "RelativeHumidityHumidifierThreshold",
            props = {
                readable = true,
                writable = true,
                supportsEventNotification = true
            },
            units = "Percentage",
            constraints = {
                minVal = 0,
                maxVal = 100,
                stepVal = 1
            },
            cbs = {
                read = read,
                write = write
            }
        }
    end
}
