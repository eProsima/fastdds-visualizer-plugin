

# WORK IN PROGRESS

Known features and bugs that will be fixed in future contributions

## Important Features

- [-] support Windows

## Features

- [-] support to discover a type using TypeObject
  - This implies to manage an internal logic so the type is not registered twice. Else it fails.
- [X] support Data Types XML files addition
- [X] support store Configuration for future runs
- [X] support array clamp/discard
- [X] filter topics
- [X] select/deselect all topics
- [-] create group topics
- [-] support string booleans to convert to numeric
- [X] support Qt settings to load/save default settings
- [-] support non bounded vector types
- [-] support keyed topics by separating keys in different topics
- [X] only show debug information when debug is enabled
- [X] work with ShapesDemo

## Improvements

- [-] list of topics/types in DialogTopics is not sorted
- [X] when opening the TopicDialog the second time it does not use the last configuration, but an older one
- [-] do not repeat xml files in list
- [X] check xml files exist before adding them to the list
- [X] implement a way that non type discovered topics are seen differently (blush/transparent) in topic list
- [X] store xml files in configuration
- [-] treat matrixes differently than vectors
- [X] arrays indexes are not in a separate label

## Bugs

- [-] when taken `flags()` from a topic in DialogTopics, sometimes it crashes because it does not found flags element
- [-] allow complex (array of structs) Data Types
  - This is a Fast DDS bug
- [X] discovering topic types does not work properly
  - [X] when a type belongs to two different topics, it does not work properly
  - [X] when publisher is already running, there are some times it does not work
- [X] xml data type files are added duplicated
