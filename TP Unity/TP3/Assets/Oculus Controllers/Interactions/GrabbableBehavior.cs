using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(Rigidbody))]
public class GrabbableBehavior : MonoBehaviour
{
    private new Rigidbody rigidbody;
    private GameObject grabber;
    private bool wasKinematic;
    private bool isHeld = false;
    public enum GrabType { None, Free, Snap };
    public GrabType grabType = GrabType.Free;

    // Start is called before the first frame update
    void Start()
    {
        rigidbody = GetComponent<Rigidbody>();
        wasKinematic = rigidbody.isKinematic;
    }

    public void TryGrab(GameObject grabber)
    {
        rigidbody.isKinematic = true;
        transform.parent = grabber.transform;
        this.grabber = grabber;
        isHeld = true;

        switch (grabType)
        {
            case GrabType.Free:
                break;

            case GrabType.Snap:
                transform.position = grabber.transform.position;
                transform.rotation = grabber.transform.rotation;
                break;

            default:
                break;
        }
    }

    public void TryRelease(GameObject grabber)
    {
        if (!grabber.Equals(this.grabber) || !isHeld) return;

        transform.parent = null;
        rigidbody.isKinematic = wasKinematic;
        isHeld = false;
    }
}
