# Augmented BST with size field

Size field definition:
 - If we define size[NIL] to be 0, then we have the identity
    ```
    size[x] = size[left[x]] + size[right[x]] + 1
    ```
## Retrieving an element with a given rank
*(CLRS, MIT Press)*


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

## Deleting in the tree
*(CLRS, MIT Press)*

**TREE-DELETE**(*T, z*)
```
if left[z] = NIL or right[z] = NIL
	then y <- z
else
	y <- TREE-SUCCESSOR(z)    
	

if left[y] != NIL
	then x <- left[y]
else
    x <- right[y] 
    

if x != NIL
	then p[x] <- p[y]
 
if p[y] = NIL
	then root[T] <- x
else if y = left[p[y]]
	then left[p[y]] <- x
else
	right[p[y]] <- x
 
if y != z
	then key[z] <- key[y]
	// If y has other fields, copy them, too.
 
return y
```