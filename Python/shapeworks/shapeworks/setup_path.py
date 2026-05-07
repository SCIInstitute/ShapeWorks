def setup_binary_path():
    import sys
    try:
        import shapeworks_py
        return  # bundled — already in site-packages
    except ImportError:
        pass
    sys.path.append("placeholder_string")
