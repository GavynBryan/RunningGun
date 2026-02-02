#pragma once

//=============================================================================
// PropertyMeta.h
//
// Backwards-compatible header for component property metadata.
// Now delegates to the generic FieldMeta system.
//=============================================================================

#include "FieldMeta.h"

// PropertyType is now an alias for FieldType
using PropertyType = FieldType;

// PropertyMeta is now an alias for FieldMeta
// Existing code using PropertyMeta will continue to work
using PropertyMeta = FieldMeta;

