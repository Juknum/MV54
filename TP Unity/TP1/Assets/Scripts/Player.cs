using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class Player : MonoBehaviour
{
    private Vector2 moveDirection = new Vector2(0, 0);
    private Vector2 lookDirection = new Vector2(0, 0);
    private CharacterController cc;

    public Camera playerCamera;
    public float rotationSpeed = 20f;
    public float moveSpeed = 1f;

    // Start is called before the first frame update
    void Start()
    {
        cc = GetComponent<CharacterController>();
    }

    // Update is called once per frame
    void Update()
    {
        if (moveDirection != Vector2.zero && cc)
            cc.Move(moveSpeed * Time.deltaTime * (moveDirection.y * transform.forward + moveDirection.x * transform.right));
        
        if (lookDirection != Vector2.zero)
        {
            transform.Rotate(rotationSpeed * Time.deltaTime * new Vector3(0, lookDirection.x, 0));
            if (playerCamera) playerCamera.transform.Rotate(rotationSpeed * Time.deltaTime * new Vector3(-lookDirection.y, 0, 0));
        }

    }

    private void OnMove(InputValue value) => moveDirection = value.Get<Vector2>();
    private void OnLook(InputValue value) => lookDirection = value.Get<Vector2>();
}
