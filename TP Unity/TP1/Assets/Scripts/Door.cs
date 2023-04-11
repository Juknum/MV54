using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Door : MonoBehaviour
{
    private AudioSource audioSource;
    private Animator animator;
    private bool isOpen = false;

    // Start is called before the first frame update
    void Start()
    {
        this.animator = GetComponent<Animator>();
        this.audioSource = GetComponent<AudioSource>();
    }

    void OnMouseDown()
    {
        if (!this.animator) return;
        if (this.animator.GetCurrentAnimatorStateInfo(0).length > this.animator.GetCurrentAnimatorStateInfo(0).normalizedTime) return;

        this.audioSource.Play();

        if (this.isOpen) this.animator.Play("CloseDoor", 0, 0);
        else this.animator.Play("OpenDoor", 0, 0);

        this.isOpen = !this.isOpen;
    }

    // Update is called once per frame
    void Update() {}
}
