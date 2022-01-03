local util = require "util"

local plugins = {}

local logger = log.getLogger("plugins")
local tinsert = table.insert

---@class AccessoryConf:table Accessory configuration.
---
---@field plugin string Plugin name.

---@class PluginConf:table Plugin configuration.
---
---@field name string Plugin name.

---@class Plugin:table Plugin.
---
---@field init fun(conf: PluginConf|nil): boolean Initialize plugin.
---@field gen fun(conf: AccessoryConf): HapAccessory|nil Generate accessory via configuration.
---@field isPending fun(): boolean Whether the accessory is waiting to be generated.

local priv = {
    plugins = {},
    accessories = {},
    done = nil
}

---Report generated bridged accessory.
---@param name string Plugin name.
---@param accessory HapAccessory Accessory.
function plugins.report(name, accessory)
    tinsert(priv.accessories, accessory)

    if priv.plugins[name].isPending() == false then
        priv.plugins[name] = nil
        if util.isEmptyTable(priv.plugins) then
            priv.done(priv.accessories)
            priv.accessories = {}
            priv.done = nil
        end
    end
end

---Load plugin.
---@param name string Plugin name.
---@param conf? PluginConf Plugin configuration.
---@return Plugin|nil
local function loadPlugin(name, conf)
    if priv.plugins[name] then
        return priv.plugins[name]
    end

    local plugin = require(name .. ".plugin")
    if util.isEmptyTable(plugin) then
        return nil
    end
    local fields = {
        init = "function",
        gen = "function",
        isPending = "function",
    }
    for k, t in pairs(fields) do
        if not plugin[k] then
            logger:error(("No field '%s' in plugin '%s'"):format(k, name))
            return nil
        end
        local _t = type(plugin[k])
        if _t ~= t then
            logger:error(("%s.%s: type error, expected %s, got %s"):format(name, k, t, _t))
            return nil
        end
    end
    if plugin.init(conf) == false then
        logger:error(("Failed to init plugin '%s'"):format(name))
        return nil
    end
    priv.plugins[name] = plugin
    return plugin
end

---Load plugins and generate bridged accessories.
---@param pluginConfs PluginConf[] Plugin configurations.
---@param accessoryConfs AccessoryConf[] Accessory configurations.
---@param done async fun(bridgedAccessories: HapAccessory[]) Function called after the bridged accessories is generated.
function plugins.start(pluginConfs, accessoryConfs, done)
    if pluginConfs then
        for _, conf in ipairs(pluginConfs) do
            loadPlugin(conf.name, conf)
        end
    end

    if accessoryConfs then
        for _, conf in ipairs(accessoryConfs) do
            local plugin = loadPlugin(conf.plugin)
            if plugin ~= nil then
                tinsert(priv.accessories, plugin.gen(conf))
            end
        end
    end

    for name, plugin in pairs(priv.plugins) do
        if plugin.isPending() == false then
            priv.plugins[name] = nil
        end
    end

    if util.isEmptyTable(priv.plugins) then
        done(priv.accessories)
        priv.accessories = {}
    else
        priv.done = done
    end
end

return plugins
