-------------------------------------------------------------------------------
  ShiftBrite_Testing
-------------------------------------------------------------------------------
  Purpose:

  Testing interfacing Arduino Leonardo and ShiftBrites in an effort to 
  eventually make a DMX controllable lighting fixture.
-------------------------------------------------------------------------------
  To Do:

  Programmability
    * Make speed and duration of lighting loop methods based on global speed
        settings.
    * Make function assignable and changeable from main loop() method.
    * Make color settings assignable and changeable from the main loop() 
        method.

  Hardware
    * Need a way to change the function without rebuilding. Next/Previous 
        button would be nice.
    * Need to figure out how to interface with DMX.
    * Need to build a bigger prototype that can share power with the LEDs and 
        power a lot of them.
-------------------------------------------------------------------------------
  Completed:

  Programmability
    * SetCurrentSceneLEDSettings method will allow setting RGBD settings for a 
        particular LED in the CurrentScene array. It converts the settings 
        array to the appropriate RGB values and calls SetCurrentSceneLEDColor.
    * SetNextSceneLEDSettings method will allow setting RGBD settings for a 
        particular LED in the NextScene array. It converts the settings array
        to the appropriate RGB values and call SetNextSceneLEDColor
    * SetCurrentSceneLEDColor method will allow setting RGB values for a 
        specified LED in the CurrentScene array.
    * SetNextSceneLEDColor method will allow setting RGB values for a specified
        LED in the NextScene array.
    * FadeToNextScene will determine the changes required to get all RGB values
        from the CurrentScene array to the NextScene array in the specified 
        number of steps, waiting the specified amount of time between each 
        step.

  Lighting Functions
    * Chaser
    * Cylon
-------------------------------------------------------------------------------
