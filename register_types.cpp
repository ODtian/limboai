/**
 * register_types.cpp
 * =============================================================================
 * Copyright 2021-2023 Serhii Snitsaruk
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 * =============================================================================
 */

#include "register_types.h"

#include "blackboard/bb_param/bb_aabb.h"
#include "blackboard/bb_param/bb_array.h"
#include "blackboard/bb_param/bb_basis.h"
#include "blackboard/bb_param/bb_bool.h"
#include "blackboard/bb_param/bb_byte_array.h"
#include "blackboard/bb_param/bb_color.h"
#include "blackboard/bb_param/bb_color_array.h"
#include "blackboard/bb_param/bb_dictionary.h"
#include "blackboard/bb_param/bb_float.h"
#include "blackboard/bb_param/bb_float_array.h"
#include "blackboard/bb_param/bb_int.h"
#include "blackboard/bb_param/bb_int_array.h"
#include "blackboard/bb_param/bb_node.h"
#include "blackboard/bb_param/bb_param.h"
#include "blackboard/bb_param/bb_plane.h"
#include "blackboard/bb_param/bb_quaternion.h"
#include "blackboard/bb_param/bb_rect2.h"
#include "blackboard/bb_param/bb_rect2i.h"
#include "blackboard/bb_param/bb_string.h"
#include "blackboard/bb_param/bb_string_array.h"
#include "blackboard/bb_param/bb_string_name.h"
#include "blackboard/bb_param/bb_transform2d.h"
#include "blackboard/bb_param/bb_transform3d.h"
#include "blackboard/bb_param/bb_variant.h"
#include "blackboard/bb_param/bb_vector2.h"
#include "blackboard/bb_param/bb_vector2_array.h"
#include "blackboard/bb_param/bb_vector2i.h"
#include "blackboard/bb_param/bb_vector3.h"
#include "blackboard/bb_param/bb_vector3_array.h"
#include "blackboard/bb_param/bb_vector3i.h"
#include "blackboard/bb_param/bb_vector4.h"
#include "blackboard/bb_param/bb_vector4i.h"
#include "blackboard/blackboard.h"
#include "bt/actions/bt_action.h"
#include "bt/actions/bt_console_print.h"
#include "bt/actions/bt_fail.h"
#include "bt/actions/bt_random_wait.h"
#include "bt/actions/bt_set_var.h"
#include "bt/actions/bt_wait.h"
#include "bt/actions/bt_wait_ticks.h"
#include "bt/behavior_tree.h"
#include "bt/bt_player.h"
#include "bt/bt_state.h"
#include "bt/bt_task.h"
#include "bt/composites/bt_composite.h"
#include "bt/composites/bt_dynamic_selector.h"
#include "bt/composites/bt_dynamic_sequence.h"
#include "bt/composites/bt_parallel.h"
#include "bt/composites/bt_random_selector.h"
#include "bt/composites/bt_random_sequence.h"
#include "bt/composites/bt_selector.h"
#include "bt/composites/bt_sequence.h"
#include "bt/conditions/bt_check_agent_property.h"
#include "bt/conditions/bt_check_trigger.h"
#include "bt/conditions/bt_check_var.h"
#include "bt/conditions/bt_condition.h"
#include "bt/decorators/bt_always_fail.h"
#include "bt/decorators/bt_always_succeed.h"
#include "bt/decorators/bt_cooldown.h"
#include "bt/decorators/bt_decorator.h"
#include "bt/decorators/bt_delay.h"
#include "bt/decorators/bt_for_each.h"
#include "bt/decorators/bt_invert.h"
#include "bt/decorators/bt_new_scope.h"
#include "bt/decorators/bt_probability.h"
#include "bt/decorators/bt_repeat.h"
#include "bt/decorators/bt_repeat_until_failure.h"
#include "bt/decorators/bt_repeat_until_success.h"
#include "bt/decorators/bt_run_limit.h"
#include "bt/decorators/bt_subtree.h"
#include "bt/decorators/bt_time_limit.h"
#include "editor/debugger/limbo_debugger.h"
#include "hsm/limbo_hsm.h"
#include "hsm/limbo_state.h"
#include "util/limbo_string_names.h"
#include "util/limbo_utility.h"

#ifdef TOOLS_ENABLED
#include "editor/debugger/behavior_tree_view.h"
#include "editor/limbo_ai_editor_plugin.h"
#endif

#include "core/object/class_db.h"
#include "core/os/memory.h"
#include "core/string/print_string.h"

static LimboUtility *_limbo_utility = nullptr;

void initialize_limboai_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		LimboDebugger::initialize();

		GDREGISTER_CLASS(LimboUtility);
		GDREGISTER_CLASS(Blackboard);

		GDREGISTER_CLASS(LimboState);
		GDREGISTER_CLASS(LimboHSM);

		GDREGISTER_ABSTRACT_CLASS(BTTask);
		GDREGISTER_CLASS(BehaviorTree);
		GDREGISTER_CLASS(BTPlayer);
		GDREGISTER_CLASS(BTState);

		GDREGISTER_CLASS(BTComposite);
		GDREGISTER_CLASS(BTSequence);
		GDREGISTER_CLASS(BTSelector);
		GDREGISTER_CLASS(BTParallel);
		GDREGISTER_CLASS(BTDynamicSequence);
		GDREGISTER_CLASS(BTDynamicSelector);
		GDREGISTER_CLASS(BTRandomSequence);
		GDREGISTER_CLASS(BTRandomSelector);

		GDREGISTER_CLASS(BTDecorator);
		GDREGISTER_CLASS(BTInvert);
		GDREGISTER_CLASS(BTAlwaysFail);
		GDREGISTER_CLASS(BTAlwaysSucceed);
		GDREGISTER_CLASS(BTDelay);
		GDREGISTER_CLASS(BTRepeat);
		GDREGISTER_CLASS(BTRepeatUntilFailure);
		GDREGISTER_CLASS(BTRepeatUntilSuccess);
		GDREGISTER_CLASS(BTRunLimit);
		GDREGISTER_CLASS(BTTimeLimit);
		GDREGISTER_CLASS(BTCooldown);
		GDREGISTER_CLASS(BTProbability);
		GDREGISTER_CLASS(BTForEach);

		GDREGISTER_CLASS(BTAction);
		GDREGISTER_CLASS(BTConsolePrint);
		GDREGISTER_CLASS(BTFail);
		GDREGISTER_CLASS(BTNewScope);
		GDREGISTER_CLASS(BTRandomWait);
		GDREGISTER_CLASS(BTSetVar);
		GDREGISTER_CLASS(BTSubtree);
		GDREGISTER_CLASS(BTWait);
		GDREGISTER_CLASS(BTWaitTicks);

		GDREGISTER_CLASS(BTCondition);
		GDREGISTER_CLASS(BTCheckAgentProperty);
		GDREGISTER_CLASS(BTCheckTrigger);
		GDREGISTER_CLASS(BTCheckVar);

		GDREGISTER_ABSTRACT_CLASS(BBParam);
		GDREGISTER_CLASS(BBInt);
		GDREGISTER_CLASS(BBBool);
		GDREGISTER_CLASS(BBFloat);
		GDREGISTER_CLASS(BBString);
		GDREGISTER_CLASS(BBVector2);
		GDREGISTER_CLASS(BBVector2i);
		GDREGISTER_CLASS(BBRect2);
		GDREGISTER_CLASS(BBRect2i);
		GDREGISTER_CLASS(BBVector3);
		GDREGISTER_CLASS(BBVector3i);
		GDREGISTER_CLASS(BBTransform2D);
		GDREGISTER_CLASS(BBVector4);
		GDREGISTER_CLASS(BBVector4i);
		GDREGISTER_CLASS(BBPlane);
		GDREGISTER_CLASS(BBQuaternion);
		GDREGISTER_CLASS(BBAabb);
		GDREGISTER_CLASS(BBBasis);
		GDREGISTER_CLASS(BBTransform3D);
		GDREGISTER_CLASS(BBColor);
		GDREGISTER_CLASS(BBStringName);
		GDREGISTER_CLASS(BBColor);
		GDREGISTER_CLASS(BBNode);
		GDREGISTER_CLASS(BBDictionary);
		GDREGISTER_CLASS(BBArray);
		GDREGISTER_CLASS(BBByteArray);
		GDREGISTER_CLASS(BBIntArray);
		GDREGISTER_CLASS(BBFloatArray);
		GDREGISTER_CLASS(BBColorArray);
		GDREGISTER_CLASS(BBStringArray);
		GDREGISTER_CLASS(BBVector2Array);
		GDREGISTER_CLASS(BBVector3Array);
		GDREGISTER_CLASS(BBVariant);

		_limbo_utility = memnew(LimboUtility);

		Engine::get_singleton()->add_singleton(Engine::Singleton("LimboUtility", LimboUtility::get_singleton()));
		LimboStringNames::create();
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		EditorPlugins::add_by_type<LimboAIEditorPlugin>();
	} // else if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
	  // GDREGISTER_CLASS(BehaviorTreeView);
	// }

#endif
}

void uninitialize_limboai_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		LimboDebugger::deinitialize();
		LimboStringNames::free();
		memdelete(_limbo_utility);
	}
}
