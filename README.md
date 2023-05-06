# BabelWires

BabelWires is a framework for data format conversion applications.

Its user interface is based on the [dataflow paradigm](https://en.wikipedia.org/wiki/Dataflow_programming):
nodes representing input files, output files and data processors can be wired together to define how data should flow from input to output, being transformed along the way. 
In flexibility and expressive power, it lies somewhere between command-line utilities (which tend to be quite fixed in functionality) and code (which is highly flexible, but unavailable to many people).
It encourages the development of generic conversion software in contrast to bespoke format-specific applications.

The BabelWires framework has no domain specific code, but provides the bulk of the domain agnostic code needed to construct this kind of application.

The defining use-case is [SeqWires](https://github.com/Malcohol/SeqWires), which supports the conversion of music sequence data between music sequencer formats.
Here's a screenshot of SeqWires:

![Screenshot showing several nodes wired together](Docs/screenshot.png "SeqWires screenshot showing several nodes wired together")

The framework provides:
* A generic way of representing data in a tree of self-describing data structures ([Feature](https://github.com/Malcohol/BabelWires/blob/main/BabelWiresLib/Features/feature.hpp))
    * The nodes of the data flow graph carry their data in one more [CompoundFeatures](https://github.com/Malcohol/BabelWires/blob/main/BabelWiresLib/Features/compoundFeature.hpp).
    * The leaves of the tree are [ValueFeatures](https://github.com/Malcohol/BabelWires/blob/main/BabelWiresLib/Features/valueFeature.hpp), which appear in the UI as rows with connectable ports.
* Abstractions for source and target formats ([SourceFileFormat](https://github.com/Malcohol/BabelWires/blob/main/BabelWiresLib/FileFormat/sourceFileFormat.hpp) and [TargetFileFormat](https://github.com/Malcohol/BabelWires/blob/main/BabelWiresLib/FileFormat/targetFileFormat.hpp))
    * Plugins register factory functions to add support for new formats.
* An abstraction for processing data ([Processor](https://github.com/Malcohol/BabelWires/blob/main/BabelWiresLib/Processors/processor.hpp))
    * A registry of factory functions is used to provide an expandable set of processing nodes. 
* A data structure describing a graph of wired nodes ([Project](https://github.com/Malcohol/BabelWires/blob/main/BabelWiresLib/Project/project.hpp)).
    * With the exception of some processors, which are internally multithreaded, BabelWires is essentially a single-threaded application.
    * With a view to future proofing, the UI accesses the Project strictly through scoped objects ([AccessModelScope](https://github.com/Malcohol/BabelWires/blob/main/BabelWiresQtUi/ModelBridge/accessModelScope.hpp) and [ModifyModelScope](https://github.com/Malcohol/BabelWires/blob/main/BabelWiresQtUi/ModelBridge/modifyModelScope.hpp)). This should make it straightforward to multithread if required.
* A version-aware [serialization system](https://github.com/Malcohol/BabelWires/blob/main/Common/Serialization/serializable.hpp) for project files.
    * All serializable classes and factory functions declare a version.
    * Every representable component of the system is associated with a universally unique identifier ([UUID](https://en.wikipedia.org/wiki/Universally_unique_identifier)). (This ensures there is no ambiguity when data is serialized and deserialized in different contexts).
* A [Qt](https://en.wikipedia.org/wiki/Qt_(software))-based user interface for manipulating projects
    * The core data management layer, which includes formats and processors, has no dependency on the UI code or Qt.
* Generic support for defining maps of key value pairs between values ([MapData](https://github.com/Malcohol/BabelWires/blob/main/BabelWiresLib/Types/Map/mapData.hpp))
    * These kinds of maps are very common in data transformation. 
* A first-class concept of failure
    * This allows the project to cope with structural changes to imported data.

Here's a screenshot of the MapEditor defining a map between two types:
![Screenshot showing the MapEditor](Docs/mapEditor.png "Screenshot of the MapEditor")

## Status

[![Build and test pipeline](https://github.com/Malcohol/BabelWires/actions/workflows/ci.yml/badge.svg)](https://github.com/Malcohol/BabelWires/actions/workflows/ci.yml)

BabelWires is under active development, but please do not expect development to be rapid.
I have very limited time to devote to this.

## Downloading and Building

Clone recursively to ensure submodules are populated:

```
git clone --recurse-submodules https://github.com/Malcohol/BabelWires.git
```

BabelWires is intended to be cross platform.
I primarily work in a GNU/Linux environment, but I'll try to ensure the MSVC/Windows build works.
Building in other environments is untested.

## License

BabelWires is licensed under the GPLv3.
See the [LICENSE](LICENSE) file.

BabelWires uses a customized version of the [Nodeeditor](https://github.com/paceholder/nodeeditor) project for its GUI.
My fork with the customizations is [here](https://github.com/Malcohol/nodeeditor), and that libary (and my customizations) are licensed under the BSD 3-Clause License.

