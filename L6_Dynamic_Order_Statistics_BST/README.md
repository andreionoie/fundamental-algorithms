# Augmented BST with size field

Size field definition:
 - If we define size[NIL] to be 0, then we have the identity
    ```
    size[x] = size[left[x]] + size[right[x]] + 1
    ```
## Retrieving an element with a given rank
*(CLRS 3rd ed 2009)*


**OS-SELECT**(*x*,*i*)

```
r <- size[left[x]] + 1
if i = r
    then return x
elseif i < r
    then return OS-SELECT(left[x],i)
else
    return OS-SELECT(right[x],i - r)
```