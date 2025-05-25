# Coding Style Guide

## File Names
- Use snake_case: `audio_system.h`, `render_manager.cpp`
- Headers use `.h`, sources use `.cpp`

## C++ Code
- **Classes/Structs**: PascalCase (`AudioSystem`, `Vector3`)
- **Functions** : PascalCase (`PlaySound()`, `GetCount()`)
- **Variables**: camelCase (`count`, `currentVolume`)
- **Namespaces**: PascalCase (`MyCore::Math`)
- **Constants**: PascalCase (`MaxBufferSize`)
- **Macros**: ALL_CAPS (`MY_DEBUG_ASSERT`)

## Class Members

### Simple Data Classes
- Use camelCase for public data members: `Vector3::x`, `Color::r`

### Complex Classes
- Use Hungarian notation with `m_` prefix for private members: `m_volume`, `m_isReady`
- Helps avoid name clashes with parameters and local variables
- Groups members together in IDE autocomplete


## Example
```cpp
// audio_system.h
class AudioSystem {
public:
    void PlayBackgroundMusic(const std::string& fileName);
private:
    float m_musicVolume;
};
```


### Another Example
```cpp
class Transform {
public:
    Vector3 position;  // Simple data - direct access
    
private:
    Matrix4 m_worldMatrix;     // Complex state - prefixed
    bool m_isDirty = false;    // Clear distinction
};
```