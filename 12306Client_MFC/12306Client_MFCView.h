
// 12306Client_MFCView.h : CMy12306Client_MFCView 类的接口
//

#pragma once

class CMy12306Client_MFCView : public CView
{
protected: // 仅从序列化创建
	CMy12306Client_MFCView();
	DECLARE_DYNCREATE(CMy12306Client_MFCView)

// 特性
public:
	CMy12306Client_MFCDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CMy12306Client_MFCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

};

#ifndef _DEBUG  // 12306Client_MFCView.cpp 中的调试版本
inline CMy12306Client_MFCDoc* CMy12306Client_MFCView::GetDocument() const
   { return reinterpret_cast<CMy12306Client_MFCDoc*>(m_pDocument); }
#endif

