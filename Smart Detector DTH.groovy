/*
 *  Copyright 2016 SmartThings
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 *
 */
 
// ------- METADATA -------
// Defines capabilities and UI for the device
metadata {
	 // Capabilities of the device
	definition (name: "Smart Detector", namespace: "triple7factor", author: "Mathew Tate") {
		// Capabilities: Actuator (device has actions), Sensor (reports information), Switch (can turn On/Off),
		//  Sound Pressure Level (volume reported in dB)
		capability "Actuator"
		capability "Switch"
		capability "Sensor"
		capability "Sound Pressure Level"
		
		// Attributes: Device state for SPL capability
		attribute "soundPressureLevel", "number"
		
		// Commands: Calibrate (device calibrates min/max sound levels)
		command "startCalibration"
	}

	// Simulator UI
	simulator {
		// Simulated switch state changes from SmartThings to the shield
		status "on":  "catchall: 0104 0000 01 01 0040 00 0A21 00 00 0000 0A 00 0A6F6E"
		status "off": "catchall: 0104 0000 01 01 0040 00 0A21 00 00 0000 0A 00 0A6F6666"
		// status "calibrate": "catchall:  0104 0000 01 01 0140 00 7DCE 00 00 0000 0B 01 0A00"

		// Status replies from the shield when the switch is physically activated
		reply "raw 0x0 { 00 00 0a 0a 6f 6e }": "catchall: 0104 0000 01 01 0040 00 0A21 00 00 0000 0A 00 0A6F6E"
		reply "raw 0x0 { 00 00 0a 0a 6f 66 66 }": "catchall: 0104 0000 01 01 0040 00 0A21 00 00 0000 0A 00 0A6F6666"
	}

	// App UI tile definitions
	tiles(scale: 2) {
		standardTile("switchTile", "device.switch", width: 2, height: 2, canChangeIcon: true) {
			state "off", label: '${currentValue}', action: "switch.on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${currentValue}', action: "switch.off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}
		
		valueTile("valueTile", "soundPressureLevel", width: 2, height: 2) {
			state "val", label:'${currentValue}', defaultState: true
		}
	}
	
	// Device preferences
	/*
	preferences {
        input "tempOffset", "number", title: "Degrees", description: "Adjust temperature by this many degrees",
              range: "*..*", displayDuringSetup: false
    }
	*/
}

// ------- ATTRIBUTES -------
// Parse incoming device messages to generate cloud events
def parse(String description) {
	// Translate the raw message from the device into a command (ie "on")
	def value = zigbee.parse(description)?.text
	
	// name = the name of the event, which is the attribute that stores the value (ie "switch")
	def name = value in ["on","off"] ? "switch" : null
	// linkText = the name of this event in the activity feed for this device
	def linkText = getLinkText(device)

	/* Default arduino handler for on/off example
	def descriptionText = getDescriptionText(description, linkText, value)
	log.debug "Desc: '${descriptionText}'"
	
	def handlerName = value
	def isStateChange = value != "ping"
	def displayed = value && isStateChange

	def result = [
		value: value,
		name: value in ["on","off"] ? "switch" : (value && value != "ping" ? "greeting" : null),
		handlerName: handlerName,
		linkText: linkText,
		descriptionText: descriptionText,
		isStateChange: isStateChange,
		displayed: displayed
	]
	*/

	log.debug result.descriptionText
	return result

}
// ------- INSTALLED -------
// Set default values for state information
def installed() {
    sendEvent(name: "switch", value: "off")
    sendEvent(name: "soundPressureLevel", value: 42.0)
}

// ------- COMMANDS -------
// Implement capabilities of the device

// Turn device on
def on() {
	zigbee.smartShield(text: "on").format()
}

// Turn device off
def off() {
	zigbee.smartShield(text: "off").format()
}

// Start calibration
def startCalibration() {
	zigbee.smartShield(text: "calibrate").format()
}