import maya.cmds as cmds

def getVertexPos(obj):
    
    # 頂点情報を格納し、最後に返すためのリスト
    vtxPosList = []
    
    # 引数に指定したオブジェクトの頂点を格納
    vertex = cmds.polyListComponentConversion(obj, toVertex = True)
    
    # 各頂点を受け取れるように展開
    vtxList = cmds.filterExpand(vertex, selectionMask = 31) # 31:ポリゴン頂点
    #vtx_list = vertex
    
    # 各頂点を返却用のリストに格納
    for vtx in vtxList:
        pos = cmds.pointPosition(vtx)
        vtxPosList.append(pos)

    return vtxPosList
    
#--------関数ここまで--------


"""動作チェック

print(getVertexPos("obj_floor0001"))

num = len(getVertexPos("obj_floor0001"))
print(num)

"""