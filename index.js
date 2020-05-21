const addon = require("bindings")("node_nif");

module.exports.getNifDetails = addon.getNifDetails;
