"""
Configuration file validation for DeepSSM.

This module provides validation for DeepSSM config files to catch
errors early with clear error messages.
"""
import os
import json
from typing import Any, Dict, List, Optional


class ConfigValidationError(Exception):
    """Raised when config validation fails."""
    pass


# Schema definition for DeepSSM config
CONFIG_SCHEMA = {
    "model_name": {"type": str, "required": True},
    "num_latent_dim": {"type": int, "required": True, "min": 1},
    "paths": {
        "type": dict,
        "required": True,
        "children": {
            "out_dir": {"type": str, "required": True},
            "loader_dir": {"type": str, "required": True},
            "aug_dir": {"type": str, "required": True},
        }
    },
    "encoder": {
        "type": dict,
        "required": True,
        "children": {
            "deterministic": {"type": bool, "required": True},
        }
    },
    "decoder": {
        "type": dict,
        "required": True,
        "children": {
            "deterministic": {"type": bool, "required": True},
            "linear": {"type": bool, "required": True},
        }
    },
    "loss": {
        "type": dict,
        "required": True,
        "children": {
            "function": {"type": str, "required": True, "choices": ["MSE", "Focal"]},
            "supervised_latent": {"type": bool, "required": True},
        }
    },
    "trainer": {
        "type": dict,
        "required": True,
        "children": {
            "epochs": {"type": int, "required": True, "min": 1},
            "learning_rate": {"type": (int, float), "required": True, "min": 0},
            "val_freq": {"type": int, "required": True, "min": 1},
            "decay_lr": {
                "type": dict,
                "required": True,
                "children": {
                    "enabled": {"type": bool, "required": True},
                    "type": {"type": str, "required": False, "choices": ["Step", "CosineAnnealing"]},
                    "parameters": {"type": dict, "required": False},
                }
            },
        }
    },
    "fine_tune": {
        "type": dict,
        "required": True,
        "children": {
            "enabled": {"type": bool, "required": True},
            "loss": {"type": str, "required": False, "choices": ["MSE", "Focal"]},
            "epochs": {"type": int, "required": False, "min": 1},
            "learning_rate": {"type": (int, float), "required": False, "min": 0},
            "val_freq": {"type": int, "required": False, "min": 1},
        }
    },
    "use_best_model": {"type": bool, "required": True},
    "tl_net": {
        "type": dict,
        "required": True,
        "children": {
            "enabled": {"type": bool, "required": True},
            "ae_epochs": {"type": int, "required": False, "min": 1},
            "tf_epochs": {"type": int, "required": False, "min": 1},
            "joint_epochs": {"type": int, "required": False, "min": 1},
            "alpha": {"type": (int, float), "required": False},
            "a_ae": {"type": (int, float), "required": False},
            "c_ae": {"type": (int, float), "required": False},
            "a_lat": {"type": (int, float), "required": False},
            "c_lat": {"type": (int, float), "required": False},
        }
    },
}


def validate_config(config_path: str) -> Dict[str, Any]:
    """
    Validate a DeepSSM configuration file.

    Args:
        config_path: Path to the JSON configuration file

    Returns:
        Validated configuration dictionary

    Raises:
        ConfigValidationError: If validation fails
        FileNotFoundError: If config file doesn't exist
        json.JSONDecodeError: If config file is not valid JSON
    """
    if not os.path.exists(config_path):
        raise FileNotFoundError(f"Config file not found: {config_path}")

    with open(config_path) as f:
        try:
            config = json.load(f)
        except json.JSONDecodeError as e:
            raise ConfigValidationError(f"Invalid JSON in config file: {e}")

    errors = _validate_dict(config, CONFIG_SCHEMA, "config")

    if errors:
        error_msg = "Config validation failed:\n" + "\n".join(f"  - {e}" for e in errors)
        raise ConfigValidationError(error_msg)

    return config


def _validate_dict(
    data: Dict[str, Any],
    schema: Dict[str, Any],
    path: str
) -> List[str]:
    """
    Recursively validate a dictionary against a schema.

    Returns:
        List of error messages (empty if valid)
    """
    errors = []

    for key, rules in schema.items():
        full_path = f"{path}.{key}"
        value = data.get(key)

        # Check required fields
        if rules.get("required", False) and key not in data:
            errors.append(f"Missing required field: {full_path}")
            continue

        if key not in data:
            continue

        # Check type
        expected_type = rules.get("type")
        if expected_type and not isinstance(value, expected_type):
            type_name = expected_type.__name__ if isinstance(expected_type, type) else str(expected_type)
            errors.append(f"Invalid type for {full_path}: expected {type_name}, got {type(value).__name__}")
            continue

        # Check min value
        if "min" in rules and isinstance(value, (int, float)):
            if value < rules["min"]:
                errors.append(f"Value too small for {full_path}: {value} < {rules['min']}")

        # Check choices
        if "choices" in rules and value not in rules["choices"]:
            errors.append(f"Invalid value for {full_path}: '{value}' not in {rules['choices']}")

        # Recurse into nested dicts
        if expected_type == dict and "children" in rules:
            errors.extend(_validate_dict(value, rules["children"], full_path))

    return errors


def validate_paths_exist(config: Dict[str, Any], check_loader_dir: bool = True) -> List[str]:
    """
    Validate that required paths in config exist.

    Args:
        config: Configuration dictionary
        check_loader_dir: Whether to check if loader_dir exists

    Returns:
        List of warning messages for missing paths
    """
    warnings = []
    paths = config.get("paths", {})

    if check_loader_dir:
        loader_dir = paths.get("loader_dir", "")
        if loader_dir and not os.path.exists(loader_dir):
            warnings.append(f"Loader directory does not exist: {loader_dir}")

    aug_dir = paths.get("aug_dir", "")
    if aug_dir and not os.path.exists(aug_dir):
        warnings.append(f"Augmentation directory does not exist: {aug_dir}")

    return warnings
