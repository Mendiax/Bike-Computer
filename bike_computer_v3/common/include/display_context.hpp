#ifndef DISPLAY_CONTEXT_HPP
#define DISPLAY_CONTEXT_HPP


/**
 * @brief This file contains structure of display
 *
 *                      MAIN
 *                       ||
 *                       \/
 * [NEW_SESSION]| SESSION       -> select bike ? and go to sesion
 * [SESSION]    | SEE_RESULT    -> display current data
 * [SEE_RESULT] | SESSION, MAIN -> "stop status" can save and go back to menu or go back to session (optionally no save)
 * [MAIN]       | NEW_SESSION
 */



class Display_Context_Interface
{
private:
    /* data */
public:
    Display_Context_Interface(/* args */);
    ~Display_Context_Interface();
    /**
     * @brief funtion that run when context is changed
     *
     */
    void on_change();
    /**
     * @brief function that operates current context
     *
     * @return Display_Context_Interface* pointer to next context
     */
    Display_Context_Interface* run();

};

Display_Context_Interface::Display_Context_Interface(/* args */)
{
}

Display_Context_Interface::~Display_Context_Interface()
{
}


#endif