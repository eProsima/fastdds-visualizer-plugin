
# WORK IN PROGRESS

Known features and bugs that will be fixed in future contributions

## Features

- [-] support to discover a type using TypeObject
  - This implies to manage an internal logic so the type is not registered twice. Else it fails.
- [-] support Data Types XML files addition
- [-] support store Configuration for future runs
- [-] support array clamp
- [-] support string booleans to convert to numeric

## Bad behaviour

- [-] list of topics/types in DialogTopics is not sorted
- [-] when opening the TopicDialog the second time it does not use the last configuration, but an older one
- [-] do not repeat xml files in list

## Bugs

- [-] when taken `flags()` from a topic in DialogTopics, sometimes it crashes because it does not found flags element
- [-] allow "complex" Data Types
  - This is a Fast DDS bug
