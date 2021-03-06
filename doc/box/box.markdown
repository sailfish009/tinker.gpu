# LATTICE

![Periodic Boundary Box](lattice.pdf "Periodic Boundary Box")

$$
\begin{cases}
   |OA| = a,\,|OB| = b,\,|OC| = c \\
   \angle bc = \alpha,\,\angle ca = \beta,\,\angle ab = \gamma \\
   OA = a\,i + 0\,j + 0\,k \\
   OB = b\cos\gamma\,i + b\sin\gamma\,j + 0\,k \\
   OC = OD\,i + DH\,j + HC\,k
\end{cases}
$$

Monoclinic: $\alpha=\gamma=\pi/2$.

Orthogonal: $\alpha=\beta=\gamma=\pi/2$.

--------------------

With $OA \cdot OC = ca\cos\beta$,

$$
OD = c\cos\beta.
$$

With $OB \cdot OC = bc\cos\alpha$,

$$
DH = \frac{c}{\sin\gamma}(\cos\alpha-\cos\beta\cos\gamma).
$$

With $HC^2 = OC^2 - OD^2 - DH^2$,

$$
HC = \frac{c}{\sin\gamma}
\sqrt{\sin^2\gamma
-\cos^2\alpha-\cos^2\beta+2\cos\alpha\cos\beta\cos\gamma}.
$$

--------------------

The pdf version was generated by
```bash
pandoc box.markdown -o box.pdf -Vfontfamily=fouriernc
```

Zhi Wang

Apr. 5, 2020.
