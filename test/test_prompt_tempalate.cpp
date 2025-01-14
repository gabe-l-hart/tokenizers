/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Third Party
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <re2/re2.h>

// Local
#include "prompt_template/types.h"

using json = nlohmann::json;
using namespace tokenizers;

// Types ///////////////////////////////////////////////////////////////////////
class TypesTest : public testing::Test {};

TEST_F(TypesTest, ModalDataToFromJson) {
  // Default construct
  ModalData modal_data;
  EXPECT_EQ(modal_data.modality, "image");
  EXPECT_EQ(modal_data.data.empty(), true);

  // serialization -> json
  json j = modal_data;
  EXPECT_EQ(j["modality"], "image");

  // deserialization -> ModalData
  ModalData modal_data_from_json = j;
  EXPECT_EQ(modal_data_from_json.modality, "image");
  EXPECT_EQ(modal_data_from_json.data.empty(), true);
}

TEST_F(TypesTest, MessageToFromJson) {
  // Construct with everything filled in
  Message message_data{"custom-role",
                       "This is a test!",
                       {
                           {{1, 2, 3}, "image"},
                           {{4, 5, 6}, "book"},
                       },
                       {{{"add",
                          "Add some things together",
                          {
                              {{"arg1", {"number", "the first arg", {}}},
                               {"arg2", {"number", "the second arg", {}}}},
                              {"arg1", "arg2"},
                          }}}}};

  // serialization -> json
  json j = message_data;
  EXPECT_EQ(j["role"], "custom-role");
  EXPECT_EQ(j["content"], "This is a test!");
  EXPECT_EQ(j["data_objects"].size(), 2);
  // TODO: Test base64 serialization for data
  EXPECT_EQ(j["data_objects"][0]["modality"], "image");
  EXPECT_EQ(j["data_objects"][1]["modality"], "book");
  EXPECT_EQ(j["tools"].size(), 1);
  EXPECT_EQ(j["tools"][0]["type"], "function");
  EXPECT_EQ(j["tools"][0]["function"]["name"], "add");
  EXPECT_EQ(j["tools"][0]["function"]["description"],
            "Add some things together");
  EXPECT_EQ(j["tools"][0]["function"]["parameters"]["properties"].size(), 2);
  EXPECT_EQ(
      j["tools"][0]["function"]["parameters"]["properties"]["arg1"]["type"],
      "number");
  EXPECT_EQ(j["tools"][0]["function"]["parameters"]["properties"]["arg1"]
             ["description"],
            "the first arg");
  EXPECT_EQ(
      j["tools"][0]["function"]["parameters"]["properties"]["arg2"]["type"],
      "number");
  EXPECT_EQ(j["tools"][0]["function"]["parameters"]["properties"]["arg2"]
             ["description"],
            "the second arg");

  // deserialization -> Message
  Message message_from_json = j;
  EXPECT_EQ(message_from_json.role, "custom-role");
  EXPECT_EQ(message_from_json.content, "This is a test!");
  EXPECT_EQ(message_from_json.data_objects.size(), 2);
  // TODO: Test base64 serialization for data
  EXPECT_EQ(message_from_json.data_objects[0].modality, "image");
  EXPECT_EQ(message_from_json.data_objects[1].modality, "book");
  EXPECT_EQ(message_from_json.tools.size(), 1);
  EXPECT_EQ(message_from_json.tools[0].type, "function");
  EXPECT_EQ(message_from_json.tools[0].function.name, "add");
  EXPECT_EQ(message_from_json.tools[0].function.description,
            "Add some things together");
  EXPECT_EQ(message_from_json.tools[0].function.parameters.properties.size(),
            2);
  EXPECT_EQ(
      message_from_json.tools[0].function.parameters.properties["arg1"].type,
      "number");
  EXPECT_EQ(message_from_json.tools[0]
                .function.parameters.properties["arg1"]
                .description,
            "the first arg");
  EXPECT_EQ(
      message_from_json.tools[0].function.parameters.properties["arg2"].type,
      "number");
  EXPECT_EQ(message_from_json.tools[0]
                .function.parameters.properties["arg2"]
                .description,
            "the second arg");
}