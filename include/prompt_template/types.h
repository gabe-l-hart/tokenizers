/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */
#pragma once

// Standard
#include <map>
#include <string>
#include <vector>

// Third Party
#include <nlohmann/json.hpp>

/*------------------------------------------------------------------------------
 * This header defines the concrete template input data types that represent the
 * input to a prompt template.
 *----------------------------------------------------------------------------*/

namespace tokenizers {

/*-- Multi-Modal Data --*/

/**
 * Types for representing raw multi-modal data
 */
typedef uint8_t byte;
typedef std::vector<byte> DataBuffer;

/**
 * A ModalData represents a piece of data associated with a particular modality.
 * For example, an image or audio. The modality is encoded as a string for
 * extensibility on a per-model basis.
 */
struct ModalData {
  DataBuffer data;
  std::string modality = "image";

  ModalData() = default;
  ModalData(DataBuffer data, std::string modality = "image")
      : data(std::move(data)), modality(std::move(modality)) {}

  // TODO: This is not correct! Currently it will just serialize data as a list
  //  of integers rather than a base64 encoded string
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ModalData, data, modality)
};

/*-- Tools --*/

/**
 * A ToolProperty represents a single parameter to a tool function
 */
struct ToolProperty {
  std::string type;
  std::string description;
  std::vector<std::string> choices; // NOTE: Not enum to avoid keyword

  ToolProperty() = default;
  ToolProperty(std::string type, std::string description,
               std::vector<std::string> choices)
      : type(std::move(type)), description(std::move(description)),
        choices(std::move(choices)) {}

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ToolProperty, type, description,
                                              choices)
};

/**
 * A ToolParameters represents the set of parameters to a tool function
 */
struct ToolParameters {
  std::string type = "object";
  std::vector<std::string> required;
  std::map<std::string, ToolProperty> properties;

  ToolParameters() = default;
  ToolParameters(std::map<std::string, ToolProperty> properties,
                 std::vector<std::string> required, std::string type = "object")
      : type(std::move(type)), required(std::move(required)),
        properties(std::move(properties)) {}

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ToolParameters, type, required,
                                              properties)
};

/**
 * A ToolFunction represents a callable function with well typed arguments
 */
struct ToolFunction {
  std::string name;
  std::string description;
  ToolParameters parameters;

  ToolFunction() = default;
  ToolFunction(std::string name, std::string description,
               ToolParameters parameters)
      : name(std::move(name)), description(std::move(description)),
        parameters(std::move(parameters)) {}

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ToolFunction, name, description,
                                              parameters)
};

/**
 * A Tool encapsulates a single tool that a model can use
 */
struct Tool {
  std::string type = "function";
  ToolFunction function;

  Tool() = default;
  Tool(ToolFunction function, std::string type = "function")
      : type(std::move(type)), function(std::move(function)) {}

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Tool, type, function)
};

/*-- Documents --*/

/**
 * A document represents a text document for a RAG request
 */
struct Document {
  std::string title;
  std::string text;

  Document() = default;
  Document(std::string title, std::string text)
      : title(std::move(title)), text(std::move(text)) {}

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Document, title, text)
};

/*-- Chat --*/

/**
 * A single chat turn message
 */
struct Message {
  std::string role;
  std::string content;
  std::vector<ModalData> data_objects;

  Message() = default;
  Message(std::string role, std::string content,
          std::vector<ModalData> data_objects)
      : role(std::move(role)), content(std::move(content)),
        data_objects(std::move(data_objects)) {}

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Message, role, content,
                                              data_objects)
};

/**
 * A full chat request message
 */
struct ChatRequest {
  std::vector<Message> messages;
  std::vector<Tool> tools;
  std::vector<Document> documents;

  /**
   * Plugins are an arbitrary key/value object that the template expander can
   * do anything with. This allows model-specific special prompt structure that
   * is not part of standard template keys.
   */
  nlohmann::json plugins;

  ChatRequest() = default;
  ChatRequest(std::vector<Message> messages, std::vector<Tool> tools,
              std::vector<Document> documents, nlohmann::json plugins)
      : messages(std::move(messages)), tools(std::move(tools)),
        documents(std::move(documents)), plugins(std::move(plugins)) {}

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChatRequest, messages, tools,
                                              documents, plugins)
};

} // namespace tokenizers