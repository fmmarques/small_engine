#if !defined(GAME_ENGINE_ABSTRACT_H)
#  define GAME_ENGINE_ABSTRACT_H

#include <input/keyboard_listener.interface.h>

#include <engine/game_state.interface.h>

//#include <graphics/

namespace engine {

enum class game_state_status { 
  UNINITIALIZED,
  INITIALIZED,
  PAUSED,
  CONTINUING,
  DEINITIALIZING
};

class game_state_abstract:
  public virtual engine::game_state_interface,
  public virtual input::keyboard_listener_interface {
public:
  game_state_abstract();
  ~game_state_abstract();

  virtual void on_initialize(); 
  virtual void on_pause();
  virtual void on_continue();
  virtual void on_deinitialize(); 

  virtual void on_event(SDL_Event *);
  virtual void on_event(SDL_KeyboardEvent *);
  
  virtual void on_render();

  virtual void on_update();

protected:
  virtual const game_state_status& get_status() const;
  virtual void set_status(game_state_status&& status);
  
private:
  game_state_status _eStatus;

};

}

#endif
