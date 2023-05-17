using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class InteractorBehavior : MonoBehaviour
{
    readonly Dictionary<string, GameObject> overlappingGrabbables = new();
    readonly Dictionary<string, GameObject> overlappingTriggers = new();

    public void OnGrabAction(InputAction.CallbackContext context)
    {
        GameObject nearest = GetNearestGrabbable();

        if (!nearest) return;
        if (context.started) nearest.GetComponent<GrabbableBehavior>().TryGrab(gameObject);
        if (context.canceled) nearest.GetComponent<GrabbableBehavior>().TryRelease(gameObject);
    }

    public void OnTriggerAction(InputAction.CallbackContext context)
    {
        GameObject nearest = GetNearestTrigger();

        if (!nearest) return;
        if (context.started) nearest.GetComponent<TriggerBehavior>().Trigger();
    }

    private void OnTriggerEnter(Collider other)
    {
        GrabbableBehavior gb = other.GetComponentInParent<GrabbableBehavior>();
        if (gb && !overlappingGrabbables.ContainsKey(gb.gameObject.name)) overlappingGrabbables.Add(gb.gameObject.name, gb.gameObject);

        TriggerBehavior tb = other.GetComponentInParent<TriggerBehavior>();
        if (tb && !overlappingTriggers.ContainsKey(tb.gameObject.name)) overlappingTriggers.Add(tb.gameObject.name, tb.gameObject);
    }

    private void OnTriggerExit(Collider other)
    {
        GrabbableBehavior gb = other.GetComponentInParent<GrabbableBehavior>();
        if (gb && overlappingGrabbables.ContainsKey(gb.gameObject.name)) overlappingGrabbables.Remove(gb.gameObject.name);

        TriggerBehavior tb = other.GetComponentInParent<TriggerBehavior>();
        if (tb && overlappingTriggers.ContainsKey(tb.gameObject.name)) overlappingTriggers.Remove(tb.gameObject.name);
    }

    private GameObject GetNearestGrabbable()
    {
        GameObject nearest = null;
        float minDist = Mathf.Infinity;

        foreach (KeyValuePair<string, GameObject> kvp in overlappingGrabbables)
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

    private GameObject GetNearestTrigger()
    {
        GameObject nearest = null;
        float minDist = Mathf.Infinity;

        foreach (KeyValuePair<string, GameObject> kvp in overlappingTriggers)
        {
            float dist = Vector3.Distance(transform.position, kvp.Value.transform.position);
            if (dist < minDist)
            {
                minDist = dist;
                nearest = kvp.Value;
            }
        }

        return nearest;
    }
}
