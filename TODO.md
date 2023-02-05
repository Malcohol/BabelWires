Bugs:
* Sometimes elements get creation twice in the UI.
  - This is probably a symptom of the weird factory re-factor in nodeeditor.
  - Consider reverting that change in my custom branch.
* The UI does not update a row directly after a failed modifier is removed, so the row stays red.

Things to check:
* Check that elements get sorted by ID when saved in projectData.
* Do non-const FeatureElement::getInputFeature and getOutputFeature really need to be public?

New features:
* "Run project" - Load all, process, save all.
* Make undo move the view to the XY and scale position of the undone command.

Unit Tests:
* Array commands with pre-existing ArraySizeModifier
* ProjectBundle serialize/deserialize - check all data is restored.

Model
* Arrays should have a getDefaultSize.
* FileFeature: Remove this and the offset hack in the FeatureCache. 
  - Instead, allow the UI to add non-model rows at top for visualization, and use that.
* Consider a coercion system so numeric types can always be assigned.

Processors:
* Provide basic processors for standard types (+, *, etc)

Refactor: 
* Consider replacing virtual deserialize() method by deserializing constructor
  - Tried this in PR #14. Breaks symmetry.
* Move some of the logic in doProcess up into FeatureElement.
* Split featureElementData into separate files - replace any dynamic casts.
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
* Try to use Value to store value in a value feature, and use it in set value modifiers.

Parallel processing:
* Not implemented, but code written with this in mind.
  - Access to the project state is already funnelled through AccessModelScope and UpdateModelScope.
  - The UI does not keep pointers to features.
* Processors would run in background threads. When they are finished, they would:
  - lock the project,
  - updating their output features,
  - process the changes.
* File loading would be similar.
* Needs new UI features, since the parts of the project can be stale which processing is underway.
* Investigate this if use-case with expensive processing ever arises.
* Ensure processor work can itself be multithreaded.

Ideas:
* Provide serializer via a registry, and move tinyxml dependency into its own lib.
  - Consider switching from XML to yaml for project files
* Require commands to be serializable. Log their serialized form (possibly as JSON).
* Don't format strings in debug logs: Use JSON so they are easy to parse.
* Processors could maybe be pure, and let the processorElement own the features.
  - Maybe they would have three pure methods: process(input, output), createDefaultInputFeature(), createDefaultOutputFeature().
  - Consider informing processors about array operations at a semantic level.
    OR giving them an old view of arrays (using the array index system already in place).
* SelectableArrays: For arrays larger than 16 elements:
  - Each element has an input drop down which selects the output array element.
  - This would be useful for complex input formats.

Types:
* Automatically provide an augmented version of each enum with a dummy value ("------" or "\_|\_" maybe?)
  - Would allow percussion / chord processor to drop events (or add them in?)