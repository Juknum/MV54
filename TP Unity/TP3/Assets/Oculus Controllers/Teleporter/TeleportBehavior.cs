using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class TeleportBehavior : MonoBehaviour
{
    public float maxDistance = 5f;
    public Material nokMaterial;
    private Material okMaterial;
    public string floorTag = "Floor";
    private bool canTeleport = false;
    private bool pointerVisible = false;
    private LineRenderer lineRenderer;
    private Vector3 destinationPoint;
    public GameObject player;

    // Start is called before the first frame update
    void Start()
    {
        lineRenderer = GetComponent<LineRenderer>();
        okMaterial = lineRenderer.material;
        HidePointer();
    }

    // Update is called once per frame
    void Update()
    {
        if (pointerVisible)
        {
            lineRenderer.SetPosition(0, transform.position);
            RaycastHit hit;

            
            if (Physics.Raycast(transform.position, transform.TransformDirection(Vector3.forward), out hit, maxDistance))
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
            else
            {
                lineRenderer.SetPosition(1, transform.position + transform.forward * maxDistance);
            }
        }
    }

    void HidePointer()
    {
        if (lineRenderer)
        {
            lineRenderer.enabled = false;
        }
        pointerVisible = false;
    }

    void ShowPointer()
    {
        if (lineRenderer)
        {
            lineRenderer.enabled = true;
        }
        pointerVisible = true;
    }

    void Teleport()
    {
        if (player)
        {
            player.transform.position = destinationPoint;
        }
    }

    public void OnTeleportAction(InputAction.CallbackContext context)
    {
        if (context.started)
        {
            ShowPointer();
        }
        if (context.canceled)
        {
            if(canTeleport)
            {
                Teleport();
            }
            HidePointer();
        }
    }

    private void FixeUpdate()
    {
        
    }

}
