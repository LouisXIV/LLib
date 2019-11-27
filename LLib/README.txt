

namespace structure

/////////////////////////////////////////////////////////////////////////////
/// now
/////////////////////////////////////////////////////////////////////////////
LLib {
	Math {
		(global)
		algebra3;
		LTranslateMatrix;
	}
	Graphics {
		CGUtility;
		{
			LCGObject;
		}
		Model3D {
			LOBJModel;
		}
		Texture {
			LRenderTextureObject;
			?LTexture (abs);
				?LTexture1D;
				?LTexture2D;
				?LTexture3D;
				?LTextureCubeMap;
		}
	}
	NV {
		fbo;
	}
	IO {
		?ImageIO {
			?LImage;   (using CxImage?)
		}
		BitmapIO{
		}
	}
	Viewer {
		LViewer;						// window control
		?LCamera;
	}
}