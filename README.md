# BabelWires

BabelWires is a set of libraries for building data format conversion applications.
It attempts to occupy a space between command-line utilities (which tend to be quite fixed in functionality), and code (which is highly flexible, but unavailable to many people).

Applications built in BabelWires allow a user to configure a conversion from a file in a source format to a file in a target format using a [dataflow](https://en.wikipedia.org/wiki/Dataflow_programming) style:
* Imported data is represented as a node with output ports
* Data to be exported is represented as a node with input ports
* Processors, which mutate the imported data, are represented as nodes with both input and output ports.

Users can create graphs of these nodes, defining how the source data is imported, processed and exported.    

Babelwires itself has no domain specific code, but provides the bulk of the domain agnostic code needed to construct this kind of application.
The defining use-case is [SeqWires](https://github.com/Malcohol/SeqWires), which supports the conversion of music sequence data between various music sequencer formats.

The framework provides:
* a generic way of representing data in a tree of self-describing data structures (Feature)
* abstractions for source and target formats
* an abstraction for processing data (Processor)
* a data structure describing a graph of wired nodes (Project)
* a versionable serialization system for projects, which writes human readable(-ish) and writable(-ish) files. 
* a Qt-based application UI for manipulating projects. (Note: The underlying data management layer has no dependency on the UI or UI framework.)
* a first-class concept of failure, allowing the project to cope with structural changes to the imported data. 

## Status

BabelWires is under active development.
Interested developers can take a look around but please note that some refactoring is planned.

Please do not expect development to be rapid.
I have very limited time to devote to this.

## Downloading and Building

Clone recursively to ensure submodules are populated:

```
git clone --recurse-submodules https://github.com/Malcohol/BabelWires.git
```

BabelWires is intended to be cross platform, but to date I have been working in a GNU/Linux environment.
The ALSA (Advanced Linux Sound Architecture) plugin is not sufficiently abstracted within the build system, so a few files will probably need to be adjusted to build on other platforms.
This will get fixed in time.

## License

BabelWires is licensed under the GPLv3.
See the [LICENSE](LICENSE) file.

BabelWires uses a customized version of the [Nodeeditor](https://github.com/paceholder/nodeeditor) project for its GUI.
My fork with the customizations is [here](https://github.com/Malcohol/nodeeditor), and that libary (and my customizations) are licensed under the BSD 3-Clause License.

