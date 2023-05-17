using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ToggleGameObject : MonoBehaviour
{
    public List<GameObject> gameObjectsToToggle;

    public void Toggle()
    {
        if (gameObjectsToToggle.Count == 0) return;

        foreach (GameObject obj in gameObjectsToToggle)
        {
            obj.SetActive(!obj.activeSelf);
        }
    }
}
