

## Protobuf
- CliServiceCommandRequest
  - string command
- CliServiceCommandResponse
  - string response
  - response code


## Input/Output sources
- USB/Bluetooth Midi SysEx (protobuf)
  - CommandRequestMessage handler calls CommandHandler and generates CommandResponseMessage
- USB/UART Serial (input/output stream)
  - Timer handler will parse input stream for commands, call CommandHandler and send response to output stream


## Data flow
  Serial Rx/Tx Service -> CliService Input Buffer

## Pseudo code
- Startup
  - Load commandMenuTree
  - Set accessLevel to 0

- Handle CommandRequestMessage
  - Run CommandHandler(msg->command)
  - Make CommandResponseMessage from response

- Timer handler
  - Read inputSource to inputBuffer
  - Parse inputBuffer
    - Look for command delimiter
      - Make commandString from inputBuffer
      - Clear inputBuffer
  - Run CommandHandler(commandString)
  - Write response to outputBuffer

- CommandHandler (string commandString)
  - if accessLevel is not set
    - Authenticate user
      - Validate commandString against password (and username)
        - if authentication fails
          -  Return response (prompt for password (and username))
        - if authentication succeeds
          - Set accessLevel
          - Return response (access granted)
  - if accessLevel is set
    - Parse commandString
      - Separate path, arguments and options
      - Produce commandRequest object
    - Validate commandRequest
      - Check for invalid characters
      - Check syntax
      - if command is not valid
        - Return response (invalid command)
    - Traverse commandMenuTree (Composite pattern) (start at currentTreePosition)
    - Check traversal result
      - if path does not exist
        - Return response (invalid path)
      - else if path is a composite
        - Update currentTreePosition
        - Return response
      - else if path is a leaf
        - Produce commandObject (factory pattern)
        - Validate accessLevel
          - Return response (access denied)
        - Validate arguments
          - Return response (invalid arguments)
        - Run Execute method
        - Return response (output of Execute method)
    - Return response string
  - else
    - Return response (access denied)
  - Format response (decorator pattern?)
  - Return response


## CommandMenuTree
Commands such as `?` and `pwd` have default implementations.

```
(root)/
├── set/
│   ├── hw/
│   │   └── rgbLed (int r, int g, int b, string format)
│   └── testMode (enum onOff)
├── get/
│   ├── hw/
│   │   ├── potmeter ()
│   │   └── hallSensor ()
│   ├── mash ()
│   └── system/
│       └── heap ()
└── action/
    ├── footswitch (string action)
    ├── toggleSwitch (string action)
    └── resetToDefault ()

```


## Command example
Command delimiter = 'CR'

```
"?"                            // List available commands and subpaths in current path
".."                           // Move back one level
"get/system"                   // Move to get/system path
"get/hw/potmeter"              // Get potmeter value
"set/hw/rgbLed 255 0 0 rgb"    // Set RGB led to red
"set/testMode 1"               // Set test mode on
"action/toggleSwitch upHold"   // Trigger toggleSwitch upHold action
```



- (root)/
  - set/
    - hw/
      - rgbLed (int r, int g, int b, string format)
    - testMode (enum onOff)
  - get/
    - hw/
      - potmeter ()
      - hallSensor ()
    - mash ()
    - system/
      - heap ()
  - action/
    - footswitch (string action)
    - toggleSwitch (string action)
    - resetToDefault ()
    