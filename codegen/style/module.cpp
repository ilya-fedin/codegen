// This file is part of Desktop App Toolkit,
// a set of libraries for developing nice desktop applications.
//
// For license and copyright information please follow this link:
// https://github.com/desktop-app/legal/blob/master/LEGAL
//
#include "codegen/style/module.h"

namespace codegen {
namespace style {
namespace structure {
namespace {

QString fullNameKey(const FullName &name) {
	return name.join('.');
}

} // namespace

Module::Module(const QString &fullpath) : fullpath_(fullpath) {
}

void Module::addIncluded(std::shared_ptr<const Module> value) {
	included_.push_back(std::move(value));
}

bool Module::addStruct(const Struct &value) {
	if (findStruct(value.name)) {
		return false;
	}
	structsByName_.insert(fullNameKey(value.name), structs_.size());
	structs_.push_back(value);
	return true;
}

const Struct *Module::findStruct(const FullName &name) const {
	if (auto result = findStructInModule(name, *this)) {
		return result;
	}
	for (const auto &module : included_) {
		if (auto result = module->findStruct(name)) {
			return result;
		}
	}
	return nullptr;
}

bool Module::addVariable(const Variable &value) {
	if (findVariable(value.name)) {
		return false;
	}
	variablesByName_.insert(fullNameKey(value.name), variables_.size());
	variables_.push_back(value);
	return true;
}

const Variable *Module::findVariable(const FullName &name, bool *outFromThisModule) const {
	if (auto result = findVariableInModule(name, *this)) {
		if (outFromThisModule) *outFromThisModule = true;
		return result;
	}
	for (const auto &module : included_) {
		if (auto result = module->findVariable(name)) {
			if (outFromThisModule) *outFromThisModule = false;
			return result;
		}
	}
	return nullptr;
}

const Struct *Module::findStructInModule(const FullName &name, const Module &module) {
	auto index = module.structsByName_.value(fullNameKey(name), -1);
	if (index < 0) {
		return nullptr;
	}
	return &module.structs_.at(index);
}

const Variable *Module::findVariableInModule(const FullName &name, const Module &module) {
	auto index = module.variablesByName_.value(fullNameKey(name), -1);
	if (index < 0) {
		return nullptr;
	}
	return &module.variables_.at(index);
}

} // namespace structure
} // namespace style
} // namespace codegen
