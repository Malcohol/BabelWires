/**
 * The command which sets a value within a map back to its default value.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

 #pragma once

 #include <BabelWiresLib/Commands/commands.hpp>
 #include <BabelWiresLib/TypeSystem/typeExp.hpp>
 #include <BabelWiresLib/Types/Map/MapProject/mapProjectDataLocation.hpp>
 
#include <BaseLib/Identifiers/identifier.hpp>

 namespace babelwires {
     class MapProject;
     class MapValue;
 
     /// Reset the value back to a default value of the given type.
     /// This can be used to change the summand of a SumType.
     class ResetMapValueCommand : public SimpleCommand<MapProject> {
       public:
         CLONEABLE(ResetMapValueCommand);
         ResetMapValueCommand(std::string commandName, MapProjectDataLocation loc, TypeExp type);
 
         virtual bool initialize(const MapProject& map) override;
         virtual void execute(MapProject& map) const override;
         virtual void undo(MapProject& map) const override;
 
       private:
         MapProjectDataLocation m_location;
         TypeExp m_type;

         // Post initialization data
 
         ValueHolder m_oldValue;
     };
 
 } // namespace babelwires
 