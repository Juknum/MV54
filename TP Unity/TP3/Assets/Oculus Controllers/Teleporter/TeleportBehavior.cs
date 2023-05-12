using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class TeleportBehavior : MonoBehaviour
{
    public GameObject player;
    public Material nokMaterial;
    public Material okMaterial;

    public string floorTag = "Floor";
    public float maxDistance = 5f;

    private bool canTeleport = false;
    private bool pointerVisible = false;

    private LineRenderer lineRenderer;
    private Vector3 destinationPoint;

    void Start()
    {
        lineRenderer = GetComponent<LineRenderer>();
        lineRenderer.enabled = false;
    }

    void FixedUpdate()
    {
        if (!pointerVisible) return;

        lineRenderer.SetPosition(0, transform.position);

        if (Physics.Raycast(transform.position, transform.TransformDirection(Vector3.forward), out RaycastHit hit, maxDistance))
        {
            if (hit.collider.gameObject.CompareTag(floorTag))
            {
                canTeleport = true;
                destinationPoint = hit.point;
                lineRenderer.material = okMaterial;
            }
            else
            {
                canTeleport = false;
                lineRenderer.material = nokMaterial;
            }

            lineRenderer.SetPosition(1, transform.position + transform.forward * hit.distance);
        }

        else lineRenderer.SetPosition(1, transform.position + transform.forward * maxDistance);
    }

    private void UpdatePointerVisibility()
    {
        if (lineRenderer) lineRenderer.enabled = !lineRenderer.enabled;
        pointerVisible = !pointerVisible;
    }

    private void Teleport()
    {
        if (!player) return;
        player.transform.position = destinationPoint;
    }

    public void OnTeleportAction(InputAction.CallbackContext context)
    {
        if (context.started) UpdatePointerVisibility();
        if (context.canceled)
        {
            if (canTeleport) Teleport();
            UpdatePointerVisibility();
        }
    }
}
