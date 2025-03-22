# BabelWires

BabelWires is an application for data format conversion.
The defining use-case is converting between formats of music sequence data, via the [SeqWires](https://github.com/Malcohol/SeqWires) plugin.
Here's a screenshot:

![Screenshot showing several nodes wired together](Docs/screenshot.png "SeqWires screenshot showing several nodes wired together")

BabelWires uses the [dataflow paradigm](https://en.wikipedia.org/wiki/Dataflow_programming) to allow users to define how data should be converted:
nodes representing input files, output files and data processors can be wired together to define how data should flow from input to output, being transformed along the way. 
In flexibility and expressive power, it lies somewhere between command-line utilities (which tend to be quite fixed in functionality) and code (which is highly flexible, but unavailable to many people).
It encourages the development of generic conversion software in contrast to bespoke format-specific applications.

Here's a screenshot of the MapEditor defining a map between two types:
![Screenshot showing the MapEditor](Docs/mapEditor.png "Screenshot of the MapEditor")

## Status

[![Build and test pipeline](https://github.com/Malcohol/BabelWires/actions/workflows/ci.yml/badge.svg)](https://github.com/Malcohol/BabelWires/actions/workflows/ci.yml)

BabelWires is intended to be cross platform but given the limited time I have available, quality is inconsistent:

| Platform | Build | Unit Tests | UI |
| -------- | --- | --- | --- | 
| GNU/Linux | Verified by CI | Verified by CI | Functional |
| Windows | Verified by CI | Verified by CI | Needs work |
| Mac OS | Manual | Manual | Needs work |

BabelWires is under active development, but please do not expect development to be rapid.
My focus is on fundimetals.

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

