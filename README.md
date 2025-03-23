# BabelWires

BabelWires is an application for data format conversion.
The defining use-case is converting between music sequence data, via the [SeqWires](https://github.com/Malcohol/SeqWires) plugin.
Here's a screenshot:

![Screenshot showing several nodes wired together](Docs/screenshot.png "SeqWires screenshot showing several nodes wired together")

Here's a screenshot of the MapEditor defining a map between two types:
![Screenshot showing the MapEditor](Docs/mapEditor.png "Screenshot of the MapEditor")

BabelWires is intended to be more flexible and expressive than command-line conversion utilities while having a lower barrier to entry than programming languages.

## Structured Data Flow

BabelWires uses a variant of the [dataflow paradigm](https://en.wikipedia.org/wiki/Dataflow_programming) I'm calling _structured data flow_.

As in conventional data flow systems, there are nodes with ports that can be wired together: In BabelWires, data enters the system at nodes corresponding to input formats, passes through data processing nodes and reaches nodes corresponding to output formats.

Less conventionally, the nodes of BabelWires have a structure defined by the data types of a type system: hence "structured" data flow.
Complex nodes with multiple ports are values of compound data types
which the UI presents as expandable, hierarchical property panels.

## Status

[![Build and test pipeline](https://github.com/Malcohol/BabelWires/actions/workflows/ci.yml/badge.svg)](https://github.com/Malcohol/BabelWires/actions/workflows/ci.yml)

BabelWires is intended to be cross platform but given the limited time I have available, quality is inconsistent:

| Platform | Build | Unit Tests | UI |
| -------- | --- | --- | --- | 
| GNU/Linux | Verified by CI | Verified by CI | Functional |
| Windows | Verified by CI | Verified by CI | Needs work |
| Mac OS | Manual | Manual | Needs work |

BabelWires is under active development, but please do not expect development to be rapid.
My focus is on fundamentals.

## Downloading and Building

Clone recursively to ensure submodules are populated:

```
git clone --recurse-submodules https://github.com/Malcohol/BabelWires.git
```

## License

BabelWires is licensed under the GPLv3.
See the [LICENSE](LICENSE) file.

BabelWires uses a customized version of the [Nodeeditor](https://github.com/paceholder/nodeeditor) project for its GUI.
My fork with the customizations is [here](https://github.com/Malcohol/nodeeditor), and that libary (and my customizations) are licensed under the BSD 3-Clause License.

