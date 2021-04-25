Bugs:
* Sometimes elements get creation twice in the UI.
  - This is probably a symptom of the weird factory re-factor in nodeeditor.
  - Consider reverting that change in my custom branch.

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
* Optional features & Unions
  - Switch via context menu option. What does the UI action look like?
  - If an unselected variant has state, does it get stored when the variant is switched. If so, where?

Refactor: 
* Move some of the logic in doProcess up into FeatureElement.
* Split featureElementData into separate files - replace any dynamic casts.
* Split up import and export - They don't have to be as symmetric as I originally intended.
  - This allows source formats to be designed to be robust on changes and target formats to be flexible.
* Split Features & Import/Export out from the project lib. 
  - In _theory_ the import/export logic could be useful on its own.
* Think about modules and dlls.
* Replace assert handler with own macros.
* Use std::format in logs and exceptions instead of streams - Better for internationalization
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
