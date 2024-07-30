"""
フィールドオブジェクト情報書き出し
"""

import maya.cmds as cmds
import math
import func_maya as func

#リロード命令
from importlib import reload
reload(func)


#取り出すアトリビュートのリスト(t=translate,r=rotate,s=scale)
attrsList = ["tx","ty","tz","rx","ry","rz","sx","sy","sz","obj_type"]

#オブジェクトの名前格納用リスト
nodesList = cmds.ls("obj*",transforms = True)

print(nodesList)



#格納先を決定する
path = cmds.fileDialog2(fileFilter="Text Files (*.txt);;All Files (*.*)",dialogStyle=1,fileMode=0,startingDirectory=func.GetCurrentDir())
                            
#ファイルへの書き込み
with open(path[0],"w") as f:    #リストの0番目にファイルパスが入っているので、0で指定している
    
    #オブジェクト総数の書き込み
    num = len(nodesList)
    print(num)
    f.write("{}\n".format(num))
    
    #各オブジェクトの情報を書き込む処理
    for node in nodesList:                            #オブジェクト毎に取り出し
        for attr in attrsList:                        #アトリビュートのリストから取り出し
            param = cmds.getAttr(node + "." + attr)   #取り出したアトリビュートを取得
            if attr == "tz":
                param *= -1#z軸修正
                
            #回転修正
            if attr == "rx":
                param = (param * math.pi) / 180
            if attr == "ry":
                param = -1*(param * math.pi) / 180
            if attr == "rz":
                param = -1*(param * math.pi) / 180
 
                
            print(param)
            f.write("{}\n".format(param))             #オブジェクトのアトリビュートをひとつ書き込み、改行する
        
        #オブジェクトタイプを格納
        objType = cmds.getAttr(node + ".obj_type")
         
        #オブジェクトタイプが98:エリアの場合、頂点座標を取得する処理 
        if objType == 98:
            vtxList = func.getVertexPos(node)
            vtxNum = len(vtxList)
            
            print(vtxNum)
            f.write("{}\n".format(vtxNum))
            
            for vtx in vtxList:  # ひとつの頂点を取り出す
                cnt = 0
                for pos in vtx:    # 頂点毎のxyz位置を順に取り出す
                    if cnt == 2:
                        pos *= -1
                        print(pos)
                        f.write("{}\n".format(pos))
                    else:
                        print(pos)
                        f.write("{}\n".format(pos))
                        cnt+=1

                
"""
オブジェクトの総数を１列目に書き出し
その後、オブジェクト毎の以下の情報を繰り返し書き出す
    pos(translate)のxyz
    rotのxyz
    sclのxyz
    オブジェクトの識別番号
    頂点のxyz（エリアオブジェクトのみ）
"""
