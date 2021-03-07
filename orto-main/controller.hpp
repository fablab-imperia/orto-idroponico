#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

enum ControllerState {WATER_CIRCULATING, WATER_MIXING, IDLE};

class Controller {

    private:
    ControllerState currentState;

    public:
    Controller();

    inline ControllerState getState() { return this->currentState; };
    inline void setState(ControllerState state) { this->currentState = state; };

};

#endif
