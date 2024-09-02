with open("asteroid.obj", "r") as f:
    lines = f.readlines()

sx = 0
sy = 0
sz = 0

cx = []
cy = []
cz = []

counter = 0
for line in lines:
    if line[0] == 'v' and line[1] != 'n':
        counter += 1
        coords = line.split()

        x = float(coords[1])
        y = float(coords[2])
        z = float(coords[3])

        cx.append(x)
        cy.append(y)
        cz.append(z)

        sx += x
        sy += y
        sz += z

avgX = sx / counter
avgY = sy / counter
avgZ = sz / counter

print(f'Estimated pos X: {avgX}, Y: {avgY}, Z: {avgZ}')
        
for i in range(len(lines)):
    if lines[i][0] == 'v' and lines[i][1] != 'n':
        coords = lines[i].split()

        x = float(coords[1]) - avgX
        y = float(coords[2]) - avgY
        z = float(coords[3]) - avgZ

        lines[i] = "v {:.6f} {:.6f} {:.6f}\n".format(x, y, z)

with open("asteroid_centered.obj", 'w') as fw:
    fw.writelines(lines)

