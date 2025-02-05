Structured Data Flow:
1. Optimization: Try to avoid excess copies of values. Remove constructor from ValueHolder which copies its value argument.
1. Consider replacing NewValueHolder by a unique_ptr variant inside ValueHolder. This might allow unique ownership to last a bit longer and avoid some unnecessary clones. (Threading probably means we can never return to this state after sharing.)

Bugs:
* Moving compound connections targets between nodes does not work properly.
  e.g. Three test record values. 
  1. Wire two together. 
  1. Expand and collapse the target node.
  1. Move the target connection to the third
  1. Expand the third record
  1. Observe that the connection gets removed.
* Connecting to a array which already has a size modifier will remove any modifiers above its default size.
  * When the array size modifier removed, it removes those modifiers even though the new connection might set the array to a sufficiently large size.
* Moving a target connection from a valid record target to a record target of a different type does not cause the modifier to fail.
* The "*" suffix of a target feature label is not always removed directly after saving.
* Array element modifications can be wrongly removed when an array value of non-default size is set from another array of equivalent size.
  - Probably the removal of the array size modifier triggers the removal of any modifier which applies to entries greater than non-default size.
* Sometimes elements get creation twice in the UI.
  - This is probably a symptom of the weird factory re-factor in nodeeditor.
  - Consider reverting that change in my custom branch.
* The UI does not update a row directly after a failed modifier is removed, so the row stays red.
* RecordWithOptionalsFeatureTest::changes test only works if the values are default. Deactivating a non-default optional should not set the value changed flag.
* Save with changes but no project file should offer "Save As", not "Save".


Things to check:
* Check that elements get sorted by ID when saved in projectData.
* Do non-const Node::getInputFeature and getOutputFeature really need to be public?

New features:
* "Run project" - Load all, process, save all.
* Make undo move the view to the XY and scale position of the undone command.

Unit Tests:
* Array commands with pre-existing ArraySizeModifier
* ProjectBundle serialize/deserialize - check all data is restored.

Model
* Consider a coercion system so numeric types can always be assigned.
* Implement RecordType Optionality::optionalDefaultActive

Processors:
* Provide basic processors for standard types (+, *, etc)

Refactor: 
* Abandon EditableValueHolder (and ValueHolder template).
  - Will have to live with unimplemented asserts.
* Consider replacing virtual deserialize() method by deserializing constructor
  - Tried this in PR #14. Breaks symmetry.
* The dispatcher should call a virtual method in the value type. That method would have to call a callback registered into the value type from the UI.
* Move some of the logic in doProcess up into Node.
* Split Features & Import/Export out from the project lib. 
  - In _theory_ the import/export logic could be useful on its own.
* Think about modules and dlls.
* Replace assert handler with own macros.
* Use std::format in logs and exceptions instead of streams - Better, esp. for internationalization
* Proper CMake usage
* Review plugin initialization
* Arrays and optional modification are special-cased in the project: Could that be handled instead by a virtual "merge" method?
  - Also, they are special cased in the removeModifierCommand. Could that be handled instead by a virtual "removeModifier" method?
  - It's slightly unfortunate to have modifierData know about commands, but overall might be worth it.
* Clean up uses of toString, operator<<, serializeToString, etc. Make clear which resolves identifiers.
* deserializeToString methods should return a tuple which includes the position after the parsed object.
* Try to sort out the various toString methods, possibly providing a "readableStream", constructed with an IdentiferRegistry::ReadAccess.
* Command::initialize could return an enum which allows a subcommand to declare that it's not needed rather than failed.

Parallel processing:
* Not implemented, but code written with this in mind.
  - Access to the project state is already funnelled through AccessModelScope and UpdateModelScope.
  - The UI does not keep pointers to features.
* Processors would run in background threads. When they are finished, they would:
  - lock the project,
  - updating their output features,
  - process the changes.
* File loading would be similar.
* Needs new UI features, since the parts of the project can be stale when processing is underway.
* Investigate this if use-case with expensive processing ever arises.
* Ensure processor work can itself be multithreaded.

Ideas:
* Provide serializer via a registry, and move tinyxml dependency into its own lib.
  - Consider switching from XML to yaml for project files
* Require commands to be serializable. Log their serialized form (possibly as JSON).
* Don't format strings in debug logs: Use JSON so they are easy to parse.
* SelectableArrays: For arrays larger than 16 elements:
  - Each element has an input drop down which selects the output array element.
  - This would be useful for complex input formats.
  - An array access processor might be a more consistent way of achieving this

Optimizations:
* Edit tree could offer "getModifiersAbove(path)" for use in some commands. (E.g. hasAncestorConnection)

Node editor dependency:
* NodeEditor has changed significantly since I took a dependency, and the version used by BabelWiresUI is now quite out of date.
* Options:
  1. Keep on as is until it starts imposing maintenance burden
  2. Update to newer version (maintain fork or try to get customizations submitted?)
  3. Replace by other framework
  4. Implement own graph UI (possibly based on NodeEditor)