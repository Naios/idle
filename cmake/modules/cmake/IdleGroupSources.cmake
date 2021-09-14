#[[
#   _____    _ _        .      .    .
#  |_   _|  | | |  .       .           .
#    | |  __| | | ___         .    .        .
#    | | / _` | |/ _ \                .
#   _| || (_| | |  __/ github.com/Naios/idle
#  |_____\__,_|_|\___| AGPL v3 (Early Access)
#
# Copyright(c) 2018 - 2021 Denis Blank <denis.blank at outlook dot com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#]]

function(idle_group_sources)
  set(arg_opt MODE)
  set(arg_single)
  set(arg_multi)
  cmake_parse_arguments(IDLE_GROUP_SOURCES "${arg_opt}" "${arg_single}"
                        "${arg_multi}" ${ARGN})

  if(IDLE_GROUP_SOURCES_MODE)
    set(WITH_SOURCE_TREE ${IDLE_GROUP_SOURCES_MODE})
  else()
    set(WITH_SOURCE_TREE "hierarchical")
  endif()

  # Skip this if WITH_SOURCE_TREE is not set (empty string).
  if(NOT ${WITH_SOURCE_TREE} STREQUAL "")
    foreach(dir ${ARGN})
      # Include all header and c files
      file(
        GLOB_RECURSE elements
        RELATIVE ${dir}
        ${dir}/*)

      foreach(element ${elements})
        # Extract filename and directory
        get_filename_component(element_name ${element} NAME)
        get_filename_component(element_dir ${element} DIRECTORY)

        if(NOT ${element_dir} STREQUAL "")
          # If the file is in a subdirectory use it as source group.
          if(${WITH_SOURCE_TREE} STREQUAL "flat")
            # Build flat structure by using only the first subdirectory.
            string(FIND ${element_dir} "/" delemiter_pos)
            if(NOT ${delemiter_pos} EQUAL -1)
              string(SUBSTRING ${element_dir} 0 ${delemiter_pos} group_name)
              source_group("${group_name}" FILES ${dir}/${element})
            else()
              # Build hierarchical structure. File is in root directory.
              source_group("${element_dir}" FILES ${dir}/${element})
            endif()
          else()
            # Use the full hierarchical structure to build source_groups.
            string(REPLACE "/" "\\" group_name ${element_dir})
            source_group("${group_name}" FILES ${dir}/${element})
          endif()
        else()
          # If the file is in the root directory, place it in the root
          # source_group.
          source_group("\\" FILES ${dir}/${element})
        endif()
      endforeach()
    endforeach()
  endif()
endfunction()
