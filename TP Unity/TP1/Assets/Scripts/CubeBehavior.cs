using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CubeBehavior : MonoBehaviour
{
    private Rigidbody rb;
    private Vector3 initPos;
    private Quaternion initRot;

    // Delay in seconds before position reset
    public float delay = 3f;

    // Start is called before the first frame update
    void Start()
    {
        this.initPos = transform.position;
        this.initRot = transform.rotation;

        this.rb = GetComponent<Rigidbody>();
        if (!this.rb) return;
        
        rb.isKinematic = true;
    }

    private void OnMouseDown() => StartCoroutine(Fall());

    private IEnumerator Fall()
    {
        // Start falling
        if (!this.rb) yield return new WaitForSeconds(delay);
        rb.isKinematic = false;

        // wait for delay
        yield return new WaitForSeconds(delay);

        // reset position
        rb.isKinematic = true;
        transform.position = this.initPos;
        transform.rotation = this.initRot;
    }

    // Update is called once per frame
    void Update() {}
}
