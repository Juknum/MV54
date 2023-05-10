using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class InputsBehavior : MonoBehaviour
{
    public Animator rightAnimator;
    public Animator leftAnimator;
    public GameObject rightThumbstick;
    public GameObject leftThumbstick;

    private Vector3 defaultPositionThumbstickLeft;

    public void Start()
    {
        if (!rightAnimator || !leftAnimator) throw new MissingReferenceException("Either one or both animators are missing");
        if (!rightThumbstick || !leftThumbstick) throw new MissingReferenceException("Either one or both thumbstick are missing");

        defaultPositionThumbstickLeft = leftThumbstick.transform.localEulerAngles;
    }

    /**
     * Buttons (A, B, X, Y)
     */
    public void OnAPressed(InputAction.CallbackContext context)
    {
        if (context.started) rightAnimator.SetBool("APressed", true);
        if (context.canceled) rightAnimator.SetBool("APressed", false);
    }

    public void OnBPressed(InputAction.CallbackContext context)
    {
        if (context.started && rightAnimator) rightAnimator.SetBool("BPressed", true);
        if (context.canceled && rightAnimator) rightAnimator.SetBool("BPressed", false);
    }

    public void OnXPressed(InputAction.CallbackContext context)
    {
        if (context.started && leftAnimator) leftAnimator.SetBool("XPressed", true);
        if (context.canceled && leftAnimator) leftAnimator.SetBool("XPressed", false);
    }
    public void OnYPressed(InputAction.CallbackContext context)
    {
        if (context.started && leftAnimator) leftAnimator.SetBool("YPressed", true);
        if (context.canceled && leftAnimator) leftAnimator.SetBool("YPressed", false);
    }

    /**
     * Front Trigger
     */
    public void OnTriggerAxisRight(InputAction.CallbackContext context) => rightAnimator.SetFloat("RightTrigger", context.ReadValue<float>());
    public void OnTriggerAxisLeft(InputAction.CallbackContext context) => leftAnimator.SetFloat("LeftTrigger", context.ReadValue<float>());

    /**
     * Side Trigger (Grip)
     */
    public void OnGripAxisRight(InputAction.CallbackContext context) => rightAnimator.SetFloat("RightGrip", context.ReadValue<float>());
    public void OnGripAxisLeft(InputAction.CallbackContext context) => leftAnimator.SetFloat("LeftGrip", context.ReadValue<float>());

    /**
     * Thumbsticks (Joysticks)
     */
    public void OnThumbstickAxisRight(InputAction.CallbackContext context)
    {
        Vector2 thumbstickValue = context.ReadValue<Vector2>();
        rightThumbstick.transform.localEulerAngles = new Vector3(thumbstickValue.y, 0, - thumbstickValue.x) * 15f;
    }

    public void OnThumbstickAxisLeft(InputAction.CallbackContext context)
    {
        Vector2 thumbstickValue = context.ReadValue<Vector2>();
        leftThumbstick.transform.localEulerAngles = new Vector3(thumbstickValue.x, 0, thumbstickValue.y) * 15f + defaultPositionThumbstickLeft;
    }

    /**
     * Presence detection (if the user is touching triggers)
     */
    public void OnTriggerTouchRight(InputAction.CallbackContext context)
    {
        if (context.started) rightAnimator.SetBool("Point", false);
        if (context.canceled) rightAnimator.SetBool("Point", true);
    }

    public void OnTriggerTouchLeft(InputAction.CallbackContext context)
    {
        if (context.started) leftAnimator.SetBool("Point", false);
        if (context.canceled) leftAnimator.SetBool("Point", true);
    }
}
