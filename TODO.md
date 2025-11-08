Bugs:
* Node dragging behavoiur is wrong after Ubuntu update (qt upgrade?)
* Moving compound connections targets between nodes does not work properly.
  e.g. Three test record values. 
  1. Wire two together. 
  1. Expand and collapse the target node.
  1. Move the target connection to the third
  1. Expand the third record
  1. Observe that the connection gets removed.
* Connecting to an array which already has a size modifier will remove any modifiers above its default size.
  * When the array size modifier removed, it removes those modifiers even though the new connection might set the array to a sufficiently large size.
* Moving a target connection from a valid record target to a record target of a different type does not cause the modifier to fail.
* The "*" suffix of a target feature label is not always removed directly after saving.
* Array element modifications can be wrongly removed when an array value of non-default size is set from another array of equivalent size.
  - Probably the removal of the array size modifier triggers the removal of any modifier which applies to entries greater than non-default size.
* The UI does not update a row directly after a failed modifier is removed, so the row stays red.
* RecordWithOptionalsFeatureTest::changes test only works if the values are default. Deactivating a non-default optional should not set the value changed flag.
* Save with changes but no project file should offer "Save As", not "Save".
* SpinBoxes do not work properly in value editor of the TestTupleType. (Open a TestTupleType in BabelWires)
* Tuples not editable in project (see TestTupleType): Only working in map editor.

Things to check:
* Check that elements get sorted by ID when saved in projectData.
* Do non-const Node::getInputFeature and getOutputFeature really need to be public?
* The complex types used by the ChordMap function (in BabelWires-Music) suggest that my attempt at flexible maps wasn't successful
  - The types and function are doing work I thought the framework would do.
  - Review the map system: Perhaps I should just simplify it.

Structured Data Flow:
1. Optimization: Try to avoid excess copies of values. Remove constructor from ValueHolder which copies its value argument.
1. Consider replacing NewValueHolder by a unique_ptr variant inside ValueHolder. This might allow unique ownership to last a bit longer and avoid some unnecessary clones. (Threading probably means we can never return to this state after sharing.)

New features:
* Make undo move the view to the XY and scale position of the undone command.
* New "Error Map Fallback" - fails a processor if the fallback is ever used.

Unit Tests:
* Array commands with pre-existing ArraySizeModifier
* ProjectBundle serialize/deserialize - check all data is restored.

Model
* Consider a coercion system so numeric types can always be assigned.
* Implement RecordType Optionality::optionalDefaultActive
* Bool and float types
* Generics - e.g. ports that can accept non-specific types, which cause the types of other ports to adapt (or something like that)
* RationalType granularity
* StringType length

Domains:
* standard numeric library
* standard string library

Refactor:
* getChildNonConst should take a Value& not a ValueHolder&
  - The current design means that tree algorithms call copyContentsAndGetNonConst repeatedly for the same container.
  - See applyToSubvalues.
  - getChild should probably be adjusted to match, although that's more painful.
* TypeRef could be a value:
  - Among other things, type constructors could just take a single array of values.
  - Additionally, I would expect to need a value containing a type at some point.
* Move some of the logic in doProcess up into Node.
* Think about modules and dlls.
  - Review plugin initialization
  - add support for removing plugins.
* Replace assert handler with own macros.
  - Provide an assert false macro that can be used in return values of arbitrary type.
* Use std::format in logs and exceptions instead of streams - Better, esp. for internationalization
* Proper CMake usage
* Arrays and optional modification are special-cased in the project: Could that be handled instead by a virtual "merge" method?
  - Also, they are special cased in the removeModifierCommand. Could that be handled instead by a virtual "removeModifier" method?
  - It's slightly unfortunate to have modifierData know about commands, but overall might be worth it.
* Clean up uses of toString, operator<<, serializeToString, etc. Make clear which resolves identifiers.
  - Could have a custom stream (or formatter) which has a lock on the identifier registry. Deadlock a danger here.
  - deserializeToString methods should return a tuple which includes the position after the parsed object.
* Command::initialize could return an enum which allows a subcommand to declare that it's not needed rather than failed.
* Can any classes be simplified using operator <=>?
* Rename TypeRef to something like TypeExpression or TypoExpr.
* Should Types be memory managed using shared_ptr, to allow purely constructed types to be collected?
* Rational should use the new Unicode utils and not require UI specialization.

UI:
* ComplexValueEditors should work for DataLocations other than just ProjectDataLocations.
* ValueModel could be a template, to avoid all the ising and asing in the subclasses.
* The dispatcher should call a virtual method in the value type. That method would have to call a callback registered into the value type from the UI.
* No way to access the context menu if a SumType was a component of a tuple type.
  - Something like: value editors could intercept the context menu and use the widget parent chain to populate the full context menu.
  - Try to define a general pattern for populating context menus.
* Whenever a context menu item can appear, it should appear, but greyed out.
  - Consider a tooltip which explains why.
* Provide a way so user can see input AND output values when they differ.
  - The obvious approach is to give Processors could have three columns, but that would use a lot of horizontal space.
  - Could make the columns selectable on a node-by-node basis.

Parallel project processing:
* Not implemented, but code written with this in mind.
  - Access to the project state is already funnelled through AccessModelScope and UpdateModelScope.
  - The UI does not store pointers to model data (I hope).
* Investigate when use-case with expensive processing arises.
* Processors would run in background threads. When they are finished, they would:
  - lock the project,
  - updating their output features,
  - process the changes.
  - UI would then need to sync changes.
* File loading would be similar.
* Needs new UI features, since the parts of the project can be stale when processing is underway.

Ideas:
* Provide serializer via a registry, and move tinyxml dependency into its own lib.
* Require commands to be serializable, to enable structured logging.
* Don't format strings in log files: Use JSON so they are easy to parse.
* SelectableArrays: For arrays larger than 16 elements:
  - Each element has an input drop down which selects the output array element.
  - This would be useful for complex input formats.
  - An array access processor might be a more consistent way of achieving this
* Add support for Type aliases so we don't have to define a type subclass in C++ just to avoid having a complex type name in the UI.
  - Could this be a variant in TypeRef which carries a string and another typeRef?

Speculative ideas:
* Programming mechanism
  - function types with map, fold, etc.
  - Consider whether existing map types could participate.

Optimizations:
* Edit tree could offer "getModifiersAbove(path)" for use in some commands. (E.g. hasAncestorConnection)
* Cache compareSubtype result.
