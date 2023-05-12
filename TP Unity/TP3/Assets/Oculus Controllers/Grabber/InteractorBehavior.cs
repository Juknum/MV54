using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class InteractorBehavior : MonoBehaviour
{
    readonly Dictionary<string, GameObject> overlappingGameObjects = new();

    public void OnGrabAction(InputAction.CallbackContext context)
    {
        GameObject nearest = GetNearestGrabbable();

        if (!nearest) return;
        if (context.started) nearest.GetComponent<GrabbableBehavior>().TryGrab(gameObject);
        if (context.canceled) nearest.GetComponent<GrabbableBehavior>().TryRelease(gameObject);
    }

    private void OnTriggerEnter(Collider other)
    {
        Debug.Log(overlappingGameObjects);
        overlappingGameObjects.Add(other.gameObject.name, other.gameObject);
    }

    private void OnTriggerExit(Collider other)
    {
        Debug.Log(overlappingGameObjects);
        overlappingGameObjects.Remove(other.gameObject.name);
    }

    private GameObject GetNearestGrabbable()
    {
        GameObject nearest = null;
        float minDist = Mathf.Infinity;

        foreach (KeyValuePair<string, GameObject> kvp in overlappingGameObjects)
        {
            if (kvp.Value.GetComponent<GrabbableBehavior>())
            {
                float dist = Vector3.Distance(transform.position, kvp.Value.transform.position);
                if (dist < minDist)
                {
                    minDist = dist;
                    nearest = kvp.Value;
                }
            }
        }

        Debug.Log(nearest);
        return nearest;
    }
}
