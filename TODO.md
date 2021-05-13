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
* Use std::filesystem::path for paths.
  - Show just filename in UI.
  - Use canonical form in XML.
* Make undo move the view to the XY and scale position of the undone command.

Unit Tests:
* Array commands with pre-existing ArraySizeModifier

Model
* Arrays should have a getDefaultSize.
* Unions
  - Switch via context menu option.
  - If an unselected variant is modified, does that modification get stored when the variant is switched. I presume only in undo stack, which is a little annoying.
* FileFeature: Remove this and the offset hack in the FeatureCache. 
  - Instead, allow the UI to add non-model rows at top for visualization, and use that.

Refactor: 
* Move some of the logic in doProcess up into FeatureElement.
* Split featureElementData into separate files - replace any dynamic casts.
* Split Features & Import/Export out from the project lib. 
  - In _theory_ the import/export logic could be useful on its own.
* Think about modules and dlls.
* Replace assert handler with own macros.
* Use std::format in logs and exceptions instead of streams - Better, esp. for internationalization
* Proper CMake usage
* Review plugin initialization

Ideas:
* Provide serializer via a registry, and move tinyxml dependency into its own lib.
  - Consider switching from XML to yaml for project files
* Require commands to be serializable. Log their serialized form (possibly as JSON).
* Don't format strings in debug logs: Use JSON so they are easy to parse.
* Processors could maybe be pure, and let the processorElement own the features.
  - Maybe they would have three pure methods: process(input, output), createDefaultInputFeature(), createDefaultOutputFeature().
  - Consider informing processors about array operations at a semantic level.
* SelectableArrays: For arrays larger than 16 elements:
  - Each element has an input drop down which selects the output array element.
  - This would be useful for complex input formats.
