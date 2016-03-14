package org.dianahep.scaroot

package reader {
  trait SchemaElement

  object SchemaNull extends SchemaElement {
    def unapply(x: Int) = if (x == 0) Some(this) else None
    override def toString() = "SchemaNull"
  }

  object SchemaBoolean extends SchemaElement {
    def unapply(x: Int) = if (x == 1) Some(this) else None
    override def toString() = "SchemaBoolean"
  }

  object SchemaInt extends SchemaElement {
    def unapply(x: Int) = if (x == 2) Some(this) else None
    override def toString() = "SchemaInt"
  }

  object SchemaLong extends SchemaElement {
    def unapply(x: Int) = if (x == 3) Some(this) else None
    override def toString() = "SchemaLong"
  }

  object SchemaFloat extends SchemaElement {
    def unapply(x: Int) = if (x == 4) Some(this) else None
    override def toString() = "SchemaFloat"
  }

  object SchemaDouble extends SchemaElement {
    def unapply(x: Int) = if (x == 5) Some(this) else None
    override def toString() = "SchemaDouble"
  }

  object SchemaString extends SchemaElement {
    def unapply(x: Int) = if (x == 6) Some(this) else None
    override def toString() = "SchemaString"
  }

  object SchemaBytes extends SchemaElement {
    def unapply(x: Int) = if (x == 7) Some(this) else None
    override def toString() = "SchemaBytes"
  }

  object SchemaArray extends SchemaElement {
    def unapply(x: Int) = if (x == 8) Some(this) else None
    override def toString() = "SchemaArray"
  }

  object SchemaMap extends SchemaElement {
    def unapply(x: Int) = if (x == 9) Some(this) else None
    override def toString() = "SchemaMap"
  }

  object SchemaRecordName extends SchemaElement {
    def unapply(x: Int) = if (x == 10) Some(this) else None
    override def toString() = "SchemaRecordName"
  }

  object SchemaRecordNamespace extends SchemaElement {
    def unapply(x: Int) = if (x == 11) Some(this) else None
    override def toString() = "SchemaRecordNamespace"
  }

  object SchemaRecordDoc extends SchemaElement {
    def unapply(x: Int) = if (x == 12) Some(this) else None
    override def toString() = "SchemaRecordDoc"
  }

  object SchemaRecordFieldName extends SchemaElement {
    def unapply(x: Int) = if (x == 13) Some(this) else None
    override def toString() = "SchemaRecordFieldName"
  }

  object SchemaRecordFieldDoc extends SchemaElement {
    def unapply(x: Int) = if (x == 14) Some(this) else None
    override def toString() = "SchemaRecordFieldDoc"
  }

  object SchemaRecordEnd extends SchemaElement {
    def unapply(x: Int) = if (x == 15) Some(this) else None
    override def toString() = "SchemaRecordEnd"
  }

  object SchemaEnumName extends SchemaElement {
    def unapply(x: Int) = if (x == 16) Some(this) else None
    override def toString() = "SchemaEnumName"
  }

  object SchemaEnumNamespace extends SchemaElement {
    def unapply(x: Int) = if (x == 17) Some(this) else None
    override def toString() = "SchemaEnumNamespace"
  }

  object SchemaEnumDoc extends SchemaElement {
    def unapply(x: Int) = if (x == 18) Some(this) else None
    override def toString() = "SchemaEnumDoc"
  }

  object SchemaEnumSymbol extends SchemaElement {
    def unapply(x: Int) = if (x == 19) Some(this) else None
    override def toString() = "SchemaEnumSymbol"
  }

  object SchemaEnumEnd extends SchemaElement {
    def unapply(x: Int) = if (x == 20) Some(this) else None
    override def toString() = "SchemaEnumEnd"
  }

  object SchemaFixedName extends SchemaElement {
    def unapply(x: Int) = if (x == 21) Some(this) else None
    override def toString() = "SchemaFixedName"
  }

  object SchemaFixedNamespace extends SchemaElement {
    def unapply(x: Int) = if (x == 22) Some(this) else None
    override def toString() = "SchemaFixedNamespace"
  }

  object SchemaFixedDoc extends SchemaElement {
    def unapply(x: Int) = if (x == 23) Some(this) else None
    override def toString() = "SchemaFixedDoc"
  }

  object SchemaFixedSize extends SchemaElement {
    def unapply(x: Int) = if (x == 24) Some(this) else None
    override def toString() = "SchemaFixedSize"
  }

  object SchemaUnionStart extends SchemaElement {
    def unapply(x: Int) = if (x == 25) Some(this) else None
    override def toString() = "SchemaUnionStart"
  }

  object SchemaUnionEnd extends SchemaElement {
    def unapply(x: Int) = if (x == 26) Some(this) else None
    override def toString() = "SchemaUnionEnd"
  }

  object SchemaReference extends SchemaElement {
    def unapply(x: Int) = if (x == 27) Some(this) else None
    override def toString() = "SchemaReference"
  }

}
